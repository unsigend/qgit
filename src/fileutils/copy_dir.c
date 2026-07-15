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
#include <fileutils.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

int copy_dir(const char *dest, const char *src)
{
    struct stat src_st, dest_st, st;
    DIR *dir;
    struct dirent *entry;
    char srcpath[PATH_MAX];
    char destpath[PATH_MAX];

    if (stat(src, &src_st) == -1 || stat(dest, &dest_st) == -1) {
        errno = ENOENT;
        return -1;
    }

    if (!S_ISDIR(src_st.st_mode) || !S_ISDIR(dest_st.st_mode)) {
        errno = ENOTDIR;
        return -1;
    }

    if ((dir = opendir(src)) == NULL)
        return -1;

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        if (snprintf(srcpath, PATH_MAX, "%s/%s", src, entry->d_name) >=
            PATH_MAX) {
            errno = ENAMETOOLONG;
            closedir(dir);
            return -1;
        }
        if (snprintf(destpath, PATH_MAX, "%s/%s", dest, entry->d_name) >=
            PATH_MAX) {
            errno = ENAMETOOLONG;
            closedir(dir);
            return -1;
        }
        if (stat(srcpath, &st) == 0 && S_ISDIR(st.st_mode)) {
            if (mkdirp(destpath, st.st_mode) == -1 ||
                copy_dir(destpath, srcpath) == -1) {
                closedir(dir);
                return -1;
            }
        } else {
            if (copy_file(destpath, srcpath) == -1) {
                closedir(dir);
                return -1;
            }
        }
    }

    closedir(dir);
    return 0;
}
