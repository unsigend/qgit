/* qgit - A simplified git like version control system
 * Copyright (C) 2025 - 2026 Qiu Yixiang
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "collection/vector.h"
#include "fs.h"
#include "index.h"
#include "repo.h"

extern int index_parse(struct index *index, const char *buf, size_t buflen);

static void index_entry_destroy(void *p)
{
  if (!p)
    return;
  struct index_entry *entry = p;
  if (entry->path)
    free(entry->path);
}

struct index *index_open(struct repo *repo)
{
  if (!repo)
    return NULL;

  char path[PATH_MAX];
  struct index *index;

  if (snprintf(path, PATH_MAX, "%s/index", repo->qgitdir) >= PATH_MAX) {
    errno = ENAMETOOLONG;
    return NULL;
  }

  if (!(index = calloc(1, sizeof(struct index))))
    return NULL;

  if (vec_init(&index->entries, sizeof(struct index_entry),
               index_entry_destroy)) {
    index_close(index);
    return NULL;
  }

  index->repo = repo;

  if (!file_exists(path)) /* not exists empty index */
  {
    index->version = IDX_FMT_VERSION;
    return index;
  }

  int fd = open(path, O_RDONLY);
  struct stat st;

  if (fd == -1) {
    index_close(index);
    return NULL;
  }

  if (fstat(fd, &st) == -1) {
    close(fd);
    index_close(index);
    return NULL;
  }

  if (st.st_size == 0) /* empty index */
  {
    close(fd);
    index->version = IDX_FMT_VERSION;
    return index;
  }

  void *buf = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (buf == MAP_FAILED) {
    close(fd);
    index_close(index);
    return NULL;
  }
  close(fd);

  if (index_parse(index, buf, st.st_size) == -1) {
    munmap(buf, st.st_size);
    index_close(index);
    return NULL;
  }
  munmap(buf, st.st_size);
  return index;
}