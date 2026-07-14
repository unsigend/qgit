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
#include <unistd.h>

int rmdirr(const char *path)
{
    struct stat st;
    DIR *dir;
    struct dirent *entry;
    char buf[PATH_MAX];

    if (stat(path, &st) == -1)
        return -1;
    if (!S_ISDIR(st.st_mode))
        return unlink(path);
    if (rmdir(path) == 0)
        return 0;

    if (errno != ENOTEMPTY && errno != EEXIST)
        return -1;

    if ((dir = opendir(path)) == NULL)
        return -1;

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        if (snprintf(buf, PATH_MAX, "%s/%s", path, entry->d_name) >= PATH_MAX) {
            errno = ENAMETOOLONG;
            closedir(dir);
            return -1;
        }
        if (rmdirr(buf) == -1) {
            closedir(dir);
            return -1;
        }
    }
    closedir(dir);
    return rmdir(path);
}