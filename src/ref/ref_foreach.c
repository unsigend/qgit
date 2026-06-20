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
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "ref.h"
#include "repo.h"
#include "sha1.h"

static int foreach (struct repo *repo, const char *path, ref_foreach_cb cb)
{
  char buf[PATH_MAX];
  DIR *dir = NULL;
  struct dirent *ent = NULL;
  unsigned char sha1[SHA1_DIGLEN];
  struct stat st;

  if (snprintf(buf, PATH_MAX, "%s/%s", repo->qgitdir, path) >= PATH_MAX) {
    errno = ENAMETOOLONG;
    return -1;
  }

  if (!(dir = opendir(buf))) {
    if (errno == ENOENT) /* skip when dir not exists */
      return 0;
    return -1;
  }

  while ((ent = readdir(dir)) != NULL) {
    if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
      continue;

    if (snprintf(buf, PATH_MAX, "%s/%s/%s", repo->qgitdir, path, ent->d_name) >=
        PATH_MAX) { /* full path */
      errno = ENAMETOOLONG;
      closedir(dir);
      return -1;
    }

    if (stat(buf, &st) == -1) {
      closedir(dir);
      return -1;
    }

    if (snprintf(buf, PATH_MAX, "%s/%s", path, ent->d_name) >=
        PATH_MAX) { /* path/name */
      errno = ENAMETOOLONG;
      closedir(dir);
      return -1;
    }

    if (S_ISDIR(st.st_mode)) /* recursive for dir */
    {
      if (foreach (repo, buf, cb) == -1) {
        closedir(dir);
        return -1;
      }

    } else /* regular ref*/
    {
      if (ref_resolve_path(repo, buf, sha1) == -1) {
        closedir(dir);
        return -1;
      }

      if (cb(buf, sha1) == -1) {
        closedir(dir);
        return -1;
      }
    }
  }

  closedir(dir);
  return 0;
}

int ref_foreach(struct repo *repo, enum ref_scope scope, ref_foreach_cb cb)
{
  if (!repo || !cb)
    return -1;

  if (scope == REF_SCOPE_ALL || scope == REF_SCOPE_BRANCHES) {
    if (foreach (repo, "refs/heads", cb) == -1)
      return -1;
  }

  if (scope == REF_SCOPE_ALL || scope == REF_SCOPE_TAGS) {
    if (foreach (repo, "refs/tags", cb) == -1)
      return -1;
  }

  return 0;
}