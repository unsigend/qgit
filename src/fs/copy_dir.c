/* miniutils - A minimal GNU coreutils implementation
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

#include "fs.h"

int copy_dir(const char *src, const char *dst)
{
  struct stat srcst, dstst;
  if (stat(src, &srcst) == -1 || stat(dst, &dstst) == -1) {
    errno = ENOENT;
    return -1;
  }
  if (!S_ISDIR(srcst.st_mode) || !S_ISDIR(dstst.st_mode)) {
    errno = ENOTDIR;
    return -1;
  }

  DIR *dir = opendir(src);
  if (dir == NULL)
    return -1;

  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
      continue;
    char srcpath[PATH_MAX];
    char dstpath[PATH_MAX];
    if (snprintf(srcpath, sizeof(srcpath), "%s/%s", src, entry->d_name) >=
        PATH_MAX) {
      errno = ENAMETOOLONG;
      closedir(dir);
      return -1;
    }
    if (snprintf(dstpath, sizeof(dstpath), "%s/%s", dst, entry->d_name) >=
        PATH_MAX) {
      errno = ENAMETOOLONG;
      closedir(dir);
      return -1;
    }
    struct stat st;
    if (stat(srcpath, &st) == 0 && S_ISDIR(st.st_mode)) {
      if (mkdirp(dstpath, st.st_mode) == -1 ||
          copy_dir(srcpath, dstpath) == -1) {
        closedir(dir);
        return -1;
      }
    } else {
      if (copy_file(srcpath, dstpath) == -1) {
        closedir(dir);
        return -1;
      }
    }
  }

  closedir(dir);
  return 0;
}