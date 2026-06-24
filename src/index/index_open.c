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
#include <unistd.h>

#include "collection/vector.h"
#include "fs.h"
#include "index.h"
#include "repo.h"

extern int index_parse(struct index *index);

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

  char buf[PATH_MAX];
  struct index *index;

  if (snprintf(buf, PATH_MAX, "%s/index", repo->qgitdir) >= PATH_MAX) {
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

  if (!file_exists(buf)) {
    index->version = IDX_FMT_VERSION;
    return index;
  }

  if (index_parse(index) == -1) {
    index_close(index);
    return NULL;
  }

  return index;
}