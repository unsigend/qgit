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
#include <libqgit/repo/refs.h>
#include <limits.h>
#include <stdio.h>

int qgit_branch_create(qgit_reference **out, qgit_repository *repo,
                       const char *branch_name, const qgit_oid *target,
                       int force)
{
    assert(out && repo && branch_name && target);

    char path[PATH_MAX];

    if (snprintf(path, PATH_MAX, "refs/heads/%s", branch_name) >= PATH_MAX) {
        errno = ENAMETOOLONG;
        return -1;
    }

    if (qgit_reference_create_oid(out, repo, path, target, force) < 0)
        return -1;

    return 0;
}
