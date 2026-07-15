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

#include "branch.h"

#include <assert.h>
#include <errno.h>
#include <libqgit/repo/repository.h>
#include <limits.h>
#include <stdio.h>
#include <unistd.h>

int qgit_branch_delete(qgit_repository *repo, const char *branch_name,
                       qgit_branch_type branch_type)
{
    assert(repo && branch_name);

    char path[PATH_MAX];

    if (qgit_branch_validate_name(branch_name) < 0)
        return -1;

    if (branch_type == QGIT_BRANCH_LOCAL) {
        if (snprintf(path, PATH_MAX, "%s/refs/heads/%s",
                     qgit_repository_path(repo), branch_name) >= PATH_MAX) {
            errno = ENAMETOOLONG;
            return -1;
        }
    } else if (branch_type == QGIT_BRANCH_REMOTE) {
        if (snprintf(path, PATH_MAX, "%s/refs/remotes/%s",
                     qgit_repository_path(repo), branch_name) >= PATH_MAX) {
            errno = ENAMETOOLONG;
            return -1;
        }
    } else {
        errno = EINVAL;
        return -1;
    }

    if (unlink(path) < 0)
        return -1;

    return 0;
}
