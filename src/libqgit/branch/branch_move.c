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
#include <fs.h>
#include <libqgit/branch.h>
#include <libqgit/error.h>
#include <libqgit/refs.h>
#include <libqgit/repository.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

int qgit_branch_move(qgit_repository *repo, const char *old_name,
                     const char *new_name, int force)
{
    assert(repo && old_name && new_name);

    char new_refname[PATH_MAX], old_refname[PATH_MAX];
    qgit_reference *old_ref;

    if (qgit_branch_lookup(&old_ref, repo, old_name, QGIT_BRANCH_LOCAL) == -1) {
        qgit_seterrno(QGITERR_REF_NOT_FOUND);
        return -1;
    }

    if (strcmp(old_name, new_name) == 0) {
        qgit_reference_free(old_ref);
        return 0;
    }

    if (snprintf(new_refname, PATH_MAX, "%s/refs/heads/%s",
                 qgit_repository_path(repo), new_name) >= PATH_MAX) {
        errno = ENAMETOOLONG;
        qgit_reference_free(old_ref);
        return -1;
    }

    if (file_exists(new_refname)) {
        if (!force) {
            qgit_reference_free(old_ref);
            qgit_seterrno(QGITERR_REFEXISTS);
            return -1;
        }
        if (qgit_branch_delete(repo, new_name, QGIT_BRANCH_LOCAL) == -1) {
            qgit_reference_free(old_ref);
            return -1;
        }
    }

    if (snprintf(old_refname, PATH_MAX, "%s/refs/heads/%s",
                 qgit_repository_path(repo), old_name) >= PATH_MAX) {
        qgit_reference_free(old_ref);
        errno = ENAMETOOLONG;
        return -1;
    }

    if (rename(old_refname, new_refname) == -1) {
        qgit_reference_free(old_ref);
        return -1;
    }

    qgit_reference_free(old_ref);
    return 0;
}
