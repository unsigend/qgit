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
#include <libqgit/repo/branch.h>
#include <libqgit/repo/refs.h>
#include <stdlib.h>
#include <string.h>

static int branch_cmp_asc(const void *a, const void *b)
{
    return strcmp(*(char **)a, *(char **)b);
}

static int branch_cmp_desc(const void *a, const void *b)
{
    return strcmp(*(char **)b, *(char **)a);
}

static void branch_free(void *data)
{
    if (!data)
        return;
    char *branch = *(char **)data;
    free(branch);
}

int qgit_branch_list(struct vector **out, qgit_repository *repo,
                     qgit_branch_type branch_type, qgit_branch_sort_type sort)
{
    assert(out && repo);

    int flag = 0;

    if (branch_type == QGIT_BRANCH_LOCAL)
        flag |= QGIT_REF_LIST_BRANCHES;
    else if (branch_type == QGIT_BRANCH_REMOTE)
        flag |= QGIT_REF_LIST_REMOTES;
    else if (branch_type == QGIT_BRANCH_ALL)
        flag |= QGIT_REF_LIST_BRANCHES | QGIT_REF_LIST_REMOTES;
    else {
        errno = EINVAL;
        return -1;
    }

    struct vector *refs;
    struct vector *branches;

    if (qgit_reference_list(&refs, repo, flag) < 0)
        return -1;

    if (vec_init(&branches, sizeof(char *), branch_free) < 0) {
        vec_free(refs);
        return -1;
    }

    for (size_t i = 0; i < vec_size(refs); i++) {
        const char *refname = *(char **)vec_at(refs, i);
        char *branchname = NULL;

        if (strncmp(refname, "refs/heads/", 11) == 0)
            branchname = strdup(refname + 11);
        else if (strncmp(refname, "refs/remotes/", 13) == 0)
            branchname = strdup(refname + 13);

        if (!branchname) {
            vec_free(refs);
            vec_free(branches);
            return -1;
        }

        if (vec_pushback(branches, &branchname) < 0) {
            free(branchname);
            vec_free(refs);
            vec_free(branches);
            return -1;
        }
    }

    vec_free(refs);

    if (sort == QGIT_BRANCH_SORT_NAME)
        vec_sort(branches, branch_cmp_asc);
    else if (sort == QGIT_BRANCH_SORT_NAME_DESC)
        vec_sort(branches, branch_cmp_desc);

    *out = branches;

    return 0;
}
