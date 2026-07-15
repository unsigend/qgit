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

#include <assert.h>
#include <errno.h>
#include <fileutils.h>
#include <libqgit/error.h>
#include <libqgit/repo/repository.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

int qgit_repository_discover(char *out, size_t out_size, const char *start_path)
{
    assert(out && out_size && start_path);

    char path[PATH_MAX];
    char *slash;
    const char *parent;

    if (start_path[0] != '/') /* relative path */
    {
        if (fabspath(start_path, path) == -1)
            return -1;
    } else if (strcmp(start_path, "/") == 0) /* '/' root */
        path[0] = '\0';
    else
        strcpy(path, start_path);

    if (snprintf(out, out_size, "%s/.qgit", path) >= (int)out_size) {
        errno = ENAMETOOLONG;
        return -1;
    }

    if (dir_exists(out))
        return 0;

    if (strcmp(start_path, "/") == 0) {
        qgit_seterror(QGITERR_REPONOTFOUND);
        return -1;
    }

    slash = strrchr(path, '/');
    if (slash == path)
        parent = "/";
    else {
        *slash = '\0';
        parent = path;
    }

    return qgit_repository_discover(out, out_size, parent);
}
