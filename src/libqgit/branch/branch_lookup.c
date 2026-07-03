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
#include <libqgit/branch.h>
#include <libqgit/error.h>
#include <libqgit/refs.h>
#include <limits.h>
#include <stdio.h>

int qgit_branch_lookup(qgit_reference **out, qgit_repository *repo,
                       const char *branch_name, qgit_branch_t branch_type)
{
    assert(out && repo && branch_name);

    qgit_reference *ref;
    char refname[PATH_MAX];

    *out = NULL;

    if (branch_type & QGIT_BRANCH_LOCAL) {
        if (snprintf(refname, PATH_MAX, "refs/heads/%s", branch_name) >=
            PATH_MAX) {
            errno = ENAMETOOLONG;
            return -1;
        }

        if (qgit_reference_lookup(&ref, repo, refname) != -1) {
            *out = ref;
            return 0;
        }
    }

    if (branch_type & QGIT_BRANCH_REMOTE) {
        if (snprintf(refname, PATH_MAX, "refs/remotes/%s", branch_name) >=
            PATH_MAX) {
            errno = ENAMETOOLONG;
            return -1;
        }

        if (qgit_reference_lookup(&ref, repo, refname) != -1) {
            *out = ref;
            return 0;
        }
    }

    qgit_seterrno(QGITERR_REF_NOT_FOUND);
    return -1;
}