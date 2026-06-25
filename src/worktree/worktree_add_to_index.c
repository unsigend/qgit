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

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "collection/vector.h"
#include "error.h"
#include "fs.h"
#include "index.h"
#include "obj/object.h"
#include "repo.h"
#include "worktree.h"

static int worktree_stat_unchanged(const struct index_entry *entry,
                                   const struct stat *st)
{
  return entry->size == (uint32_t)st->st_size &&
         entry->mtime == (uint32_t)st->st_mtime &&
         entry->mtime_nsec == (uint32_t)stat_mtime_nsec(st) &&
         entry->ctime == (uint32_t)st->st_ctime &&
         entry->ctime_nsec == (uint32_t)stat_ctime_nsec(st) &&
         entry->ino == (uint32_t)st->st_ino &&
         entry->dev == (uint32_t)st->st_dev;
}

int worktree_add_to_index(struct index *index, const char *path)
{
  if (!index || !path || !*path)
    return -1;

  char realpath[PATH_MAX];
  if (snprintf(realpath, PATH_MAX, "%s/%s", index->repo->worktree, path) >=
      PATH_MAX) {
    errno = ENAMETOOLONG;
    return -1;
  }

  struct stat st;

  if (stat(realpath, &st) == -1)
    return -1;

  if (S_ISDIR(st.st_mode)) /* recursive for directory */
  {
    DIR *dir = NULL;
    struct dirent *dirent = NULL;

    dir = opendir(realpath);
    if (dir == NULL)
      return -1;

    while ((dirent = readdir(dir)) != NULL) {
      if (strcmp(dirent->d_name, ".") == 0 ||
          strcmp(dirent->d_name, "..") == 0 ||
          strcmp(dirent->d_name, ".qgit") == 0 ||
          strcmp(dirent->d_name, ".git") == 0)
        continue;

      char *subpath; /* use heap to avoid stack overflow */
      size_t subpathlen;

      if ((subpathlen = snprintf(NULL, 0, "%s/%s", path, dirent->d_name)) >=
          PATH_MAX) {
        errno = ENAMETOOLONG;
        closedir(dir);
        return -1;
      }
      subpath = malloc(subpathlen + 1);
      if (!subpath) {
        closedir(dir);
        return -1;
      }

      if (snprintf(subpath, subpathlen + 1, "%s/%s", path, dirent->d_name) >=
          PATH_MAX) {
        free(subpath);
        closedir(dir);
        return -1;
      }
      subpath[subpathlen] = '\0';

      if (worktree_add_to_index(index, subpath) == -1) {
        free(subpath);
        closedir(dir);
        return -1;
      }

      free(subpath);
    }
    closedir(dir);
  } else /* regular file */
  {
    struct object *obj = NULL;
    void *buf = NULL;
    int fd = -1;
    struct index_entry *existing;

    if (!S_ISREG(st.st_mode)) {
      setqerrno(QE_INVALIDOBJ);
      return -1;
    }

    existing = index_find(index, path, IDXE_STAGE_NORMAL);
    if (existing && worktree_stat_unchanged(existing, &st))
      return 0;

    if ((fd = open(realpath, O_RDONLY)) == -1)
      return -1;

    if (st.st_size) {
      if ((buf = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0)) ==
          MAP_FAILED) {
        close(fd);
        return -1;
      }
      close(fd);

      if (!(obj = obj_create(buf, st.st_size, OBJ_BLOB))) {
        munmap(buf, st.st_size);
        return -1;
      }
      munmap(buf, st.st_size);
    } else {
      close(fd);
      if (!(obj = obj_create(NULL, 0, OBJ_BLOB)))
        return -1;
    }

    if (obj_write(obj, index->repo) == -1) {
      obj_close(obj);
      return -1;
    }

    struct index_entry entry;
    sha1_copy(obj->sha1, entry.sha1);
    obj_close(obj);

    entry.ctime = st.st_ctime;
    entry.ctime_nsec = stat_ctime_nsec(&st);
    entry.mtime = st.st_mtime;
    entry.mtime_nsec = stat_mtime_nsec(&st);
    entry.dev = st.st_dev;
    entry.ino = st.st_ino;
    entry.mode = (IDXE_TYPE_REGULAR << 12) | (st.st_mode & 0777);
    entry.uid = st.st_uid;
    entry.gid = st.st_gid;
    entry.size = st.st_size;

    entry.assume_valid = 0;
    entry.extended = 0;
    entry.stage = IDXE_STAGE_NORMAL;
    entry.path = strdup(path);
    if (!entry.path)
      return -1;

    if (index_add(index, &entry) == -1) {
      free(entry.path);
      return -1;
    }

    return 0;
  }

  return 0;
}