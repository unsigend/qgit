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

#include "repository.h"

#include <assert.h>
#include <errno.h>
#include <fs.h>
#include <libqgit/error.h>
#include <libqgit/repository.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

int qgit_repository_discover(char *repo_path, size_t size,
                             const char *start_path)
{
    assert(repo_path && size && start_path);

    char path[PATH_MAX];
    char *slash;
    const char *parent;

    if (start_path[0] != '/') {
        if (fabspath(start_path, path) == -1)
            return -1;
    } else if (strcmp(start_path, "/") == 0)
        path[0] = '\0';
    else
        strcpy(path, start_path);

    if (snprintf(repo_path, size, "%s/.qgit", path) >= (int)size) {
        errno = ENAMETOOLONG;
        return -1;
    }

    if (dir_exists(repo_path))
        return 0;

    if (strcmp(start_path, "/") == 0) {
        qgit_seterrno(QGITERR_REPO_NOT_FOUND);
        return -1;
    }

    slash = strrchr(path, '/');
    if (slash == path)
        parent = "/";
    else {
        *slash = '\0';
        parent = path;
    }

    return qgit_repository_discover(repo_path, size, parent);
}