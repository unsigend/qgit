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

#include <errno.h>
#include <limits.h>
#include <string.h>
#include <sys/stat.h>

int mkdirp(const char *path, mode_t mode)
{
    if (mkdir(path, mode) == 0)
        return 0;

    if (errno == EEXIST) {
        struct stat st;
        if (stat(path, &st) == 0 && S_ISDIR(st.st_mode))
            return 0;
    }

    if (errno != ENOENT)
        return -1;

    char parent[PATH_MAX];
    if (strlen(path) >= PATH_MAX) {
        errno = ENAMETOOLONG;
        return -1;
    }
    strncpy(parent, path, PATH_MAX - 1);
    parent[PATH_MAX - 1] = '\0';
    char *slash = strrchr(parent, '/');
    if (slash == NULL) {
        errno = EINVAL;
        return -1;
    }

    *slash = '\0';
    if (mkdirp(parent, mode) == -1)
        return -1;

    return mkdir(path, mode);
}