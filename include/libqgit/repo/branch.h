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

#ifndef LIBQGIT_REPO_BRANCH_H
#define LIBQGIT_REPO_BRANCH_H

#include <collection/vector.h>
#include <libqgit/common.h>
#include <libqgit/oid.h>
#include <libqgit/types.h>

QGIT_BEGIN_DECLS

/* Sort order for qgit_branch_list output. */
typedef enum {
    QGIT_BRANCH_SORT_NONE = 0,      /* filesystem order */
    QGIT_BRANCH_SORT_NAME = 1,      /* ascending alphabetical */
    QGIT_BRANCH_SORT_NAME_DESC = 2, /* descending alphabetical */
} qgit_branch_sort_type;

/**
 * Create a new local branch pointing at a target commit.
 *
 * Writes a direct reference under refs/heads/<branch_name>. If force
 * is non-zero and a branch with that name already exists, it is replaced.
 * The returned reference must be freed with qgit_reference_free.
 *
 * @param out         output pointer to receive the created reference, must not
 *                    be NULL
 * @param repo        repository where to store the branch, must not be NULL
 * @param branch_name short branch name (e.g. "main"), must not be NULL
 * @param target      OID of the commit the branch will point to, must not be
 *                    NULL
 * @param force       if non-zero, overwrite an existing branch with the same
 *                    name
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int)
qgit_branch_create(qgit_reference **out, qgit_repository *repo,
                   const char *branch_name, const qgit_oid *target, int force);

/**
 * Delete an existing branch by its short name.
 *
 * Only local branches (QGIT_BRANCH_LOCAL) may be deleted in the current
 * implementation, remote branches are reserved for future use.
 *
 * @param repo        repository where the branch lives, must not be NULL
 * @param branch_name short branch name to delete (e.g. "feature-x"), must not
 *                    be NULL
 * @param branch_type QGIT_BRANCH_LOCAL or QGIT_BRANCH_REMOTE
 * @return 0 on success, -1 if not found or on error and sets errno
 */
QGIT_EXTERN(int)
qgit_branch_delete(qgit_repository *repo, const char *branch_name,
                   qgit_branch_type branch_type);

/**
 * Rename an existing local branch.
 *
 * If force is non-zero and a branch named new_name already exists, it is
 * replaced. Only local branches may be renamed.
 *
 * @param repo     repository where the branch lives, must not be NULL
 * @param old_name current short branch name, must not be NULL
 * @param new_name new short branch name, must not be NULL
 * @param force    if non-zero, overwrite an existing branch named new_name
 * @return 0 on success, -1 if not found or on error and sets errno
 */
QGIT_EXTERN(int)
qgit_branch_move(qgit_repository *repo, const char *old_name,
                 const char *new_name, int force);

/**
 * Look up a branch reference by short name and type.
 *
 * The returned reference must be freed with qgit_reference_free.
 *
 * @param out         output pointer to receive the reference, must not be NULL
 * @param repo        repository to search, must not be NULL
 * @param branch_name short branch name (e.g. "main"), must not be NULL
 * @param branch_type QGIT_BRANCH_LOCAL or QGIT_BRANCH_REMOTE
 * @return 0 on success, -1 if not found or on error and sets errno
 */
QGIT_EXTERN(int)
qgit_branch_lookup(qgit_reference **out, qgit_repository *repo,
                   const char *branch_name, qgit_branch_type branch_type);

/**
 * Build a sorted list of branch short names in the repository.
 *
 * The output is a newly allocated vector of char* entries, one per short
 * branch name (e.g. "main", not "refs/heads/main"). The caller must free
 * the strings and the vector with vec_free when done.
 *
 * @param out         output pointer to receive a vector of char*, must not be
 *                    NULL
 * @param repo        repository to list branches from, must not be NULL
 * @param branch_type QGIT_BRANCH_LOCAL, QGIT_BRANCH_REMOTE, or QGIT_BRANCH_ALL
 * @param sort        sort order for the returned names
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int)
qgit_branch_list(struct vector **out, qgit_repository *repo,
                 qgit_branch_type branch_type, qgit_branch_sort_type sort);

/**
 * Return the short branch name from a branch reference.
 *
 * Strips the "refs/heads/" or "refs/remotes/" prefix. The returned
 * pointer is owned by the reference and is valid until qgit_reference_free
 * is called.
 *
 * @param ref branch reference, must not be NULL
 * @return short branch name, or NULL if ref is not a branch reference
 */
QGIT_EXTERN(const char *) qgit_branch_name(const qgit_reference *ref);

/**
 * Test whether a branch reference is the currently checked-out HEAD.
 *
 * @param ref branch reference to test, must not be NULL
 * @return 1 if the branch is HEAD, 0 if not, -1 on error and sets errno
 */
QGIT_EXTERN(int) qgit_branch_is_head(const qgit_reference *ref);

QGIT_END_DECLS

#endif
