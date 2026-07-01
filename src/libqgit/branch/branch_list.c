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
#include <collection/vector.h>
#include <libqgit/branch.h>
#include <stdlib.h>
#include <string.h>

struct ctx {
    struct vector *vec;
    qgit_branch_t type;
};

static int cmp_branch_asc(const void *a, const void *b)
{
    const char *branch_a = *(const char **)a;
    const char *branch_b = *(const char **)b;
    return strcmp(branch_a, branch_b);
}

static int cmp_branch_desc(const void *a, const void *b)
{
    const char *branch_a = *(const char **)a;
    const char *branch_b = *(const char **)b;
    return strcmp(branch_b, branch_a);
}

static int callback(const char *name, void *payload)
{
    struct ctx *ctx = payload;
    char *branchname;

    if (strncmp(name, "refs/heads/", 11) == 0 &&
        (ctx->type & QGIT_BRANCH_LOCAL)) {
        branchname = strdup(name + 11);
    } else if (strncmp(name, "refs/remotes/", 13) == 0 &&
               (ctx->type & QGIT_BRANCH_REMOTE)) {
        branchname = strdup(name + 13);
    } else
        return 0;

    if (!branchname)
        return -1;

    if (vec_pushback(ctx->vec, &branchname) == -1) {
        free(branchname);
        return -1;
    }

    return 0;
}

int qgit_branch_list(struct vector *out, qgit_repository *repo,
                     qgit_branch_t branch_type, qgit_branch_sort_t sort)
{
    assert(out && repo);

    if (vec_init(out, sizeof(char *), free) == -1)
        return -1;

    struct ctx ctx = {
        .vec = out,
        .type = branch_type,
    };

    if (qgit_reference_foreach(repo, callback, &ctx) == -1) {
        vec_fini(out);
        return -1;
    }

    if (sort == QGIT_BRANCH_SORT_NAME) {
        vec_sort(out, cmp_branch_asc);
    } else if (sort == QGIT_BRANCH_SORT_NAME_DESC) {
        vec_sort(out, cmp_branch_desc);
    }

    return 0;
}