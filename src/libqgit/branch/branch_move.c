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
#include <fileutils.h>
#include <libqgit/repo/refs.h>
#include <libqgit/repo/repository.h>
#include <limits.h>
#include <stdio.h>

int qgit_branch_move(qgit_repository *repo, const char *old_name,
                     const char *new_name, int force)
{
    assert(repo && old_name && new_name);

    char oldpath[PATH_MAX], newpath[PATH_MAX];
    qgit_reference *head;
    char oldref[PATH_MAX], newref[PATH_MAX];

    if (qgit_branch_validate_name(old_name) < 0 ||
        qgit_branch_validate_name(new_name) < 0)
        return -1;

    if (qgit_repository_head(&head, repo) < 0)
        return -1;

    if (snprintf(oldpath, PATH_MAX, "%s/refs/heads/%s",
                 qgit_repository_path(repo), old_name) >= PATH_MAX) {
        qgit_reference_free(head);
        errno = ENAMETOOLONG;
        return -1;
    }

    if (snprintf(oldref, PATH_MAX, "refs/heads/%s", old_name) >= PATH_MAX) {
        qgit_reference_free(head);
        errno = ENAMETOOLONG;
        return -1;
    }

    if (snprintf(newpath, PATH_MAX, "%s/refs/heads/%s",
                 qgit_repository_path(repo), new_name) >= PATH_MAX) {
        qgit_reference_free(head);
        errno = ENAMETOOLONG;
        return -1;
    }

    if (snprintf(newref, PATH_MAX, "refs/heads/%s", new_name) >= PATH_MAX) {
        qgit_reference_free(head);
        errno = ENAMETOOLONG;
        return -1;
    }

    if (file_exists(newpath) && !force) {
        qgit_seterror(QGITERR_BRANCHEXISTS);
        qgit_reference_free(head);
        return -1;
    }

    if (rename(oldpath, newpath) < 0) {
        qgit_reference_free(head);
        return -1;
    }

    if (qgit_reference_type(head) == QGIT_REF_SYMBOLIC &&
        strcmp(qgit_reference_target(head), oldref) == 0) {
        if (qgit_reference_set_target(head, newref) < 0) {
            rename(newpath, oldpath);
            qgit_reference_free(head);
            return -1;
        }
    }

    qgit_reference_free(head);

    return 0;
}
