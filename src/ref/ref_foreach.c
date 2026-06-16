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
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "ref.h"
#include "sha1.h"

int ref_foreach(struct repo *repo, const char *prefix, ref_cb cb, void *arg)
{
  if (!repo || !prefix || !cb) {
    errno = EINVAL;
    return -1;
  }

  char path[PATH_MAX];
  if (snprintf(path, PATH_MAX, "%s/%s", repo->gitdir, prefix) >= PATH_MAX) {
    errno = ENAMETOOLONG;
    return -1;
  }

  DIR *dir = opendir(path);
  if (dir == NULL) {
    if (errno == ENOENT)
      return 0;
    return -1;
  }

  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
      continue;

    if (snprintf(path, PATH_MAX, "%s/%s/%s", repo->gitdir, prefix,
                 entry->d_name) >= PATH_MAX) {
      errno = ENAMETOOLONG;
      closedir(dir);
      return -1;
    }

    struct stat st;
    if (stat(path, &st) != -1 && S_ISDIR(st.st_mode)) {
      if (snprintf(path, PATH_MAX, "%s/%s", prefix, entry->d_name) >=
          PATH_MAX) {
        errno = ENAMETOOLONG;
        closedir(dir);
        return -1;
      }
      if (ref_foreach(repo, path, cb, arg) == -1) {
        closedir(dir);
        return -1;
      }

    } else {
      if (snprintf(path, PATH_MAX, "%s/%s", prefix, entry->d_name) >=
          PATH_MAX) {
        errno = ENAMETOOLONG;
        closedir(dir);
        return -1;
      }

      unsigned char sha1[SHA1_DIGEST_LENGTH];
      if (ref_read(repo, path, sha1) == -1) {
        closedir(dir);
        return -1;
      }
      if (cb(path, sha1, arg) == -1) {
        closedir(dir);
        return -1;
      }
    }
  }

  closedir(dir);
  return 0;
}