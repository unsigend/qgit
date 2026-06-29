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
#include <limits.h>
#include <stdio.h>

int qgit_repository_head_unborn(qgit_repository *repo)
{
    assert(repo);

    char buf[PATH_MAX];
    char path[PATH_MAX];
    char ref[PATH_MAX];

    if (qgit_repository_head_content(repo, buf, PATH_MAX) == -1)
        return -1;
    if (sscanf(buf, "ref: %s", ref) != 1)
        return 0;
    if (snprintf(path, PATH_MAX, "%s/%s", repo->repodir, ref) >= PATH_MAX) {
        errno = ENAMETOOLONG;
        return -1;
    }
    return file_exists(path) ? 0 : 1;
}