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

#ifndef LIBQGIT_BRANCH_H
#define LIBQGIT_BRANCH_H

#include <collection/vector.h>
#include <libqgit/common.h>
#include <libqgit/db/oid.h>

typedef enum {
    QGIT_BRANCH_LOCAL = 1,  /* Local branch */
    QGIT_BRANCH_REMOTE = 2, /* Remote branch */
    QGIT_BRANCH_ALL = QGIT_BRANCH_LOCAL | QGIT_BRANCH_REMOTE,
} qgit_branch_t;

typedef enum {
    QGIT_BRANCH_SORT_NONE = 0,      /* Filesystem order */
    QGIT_BRANCH_SORT_NAME = 1,      /* Ascending alphabetical order */
    QGIT_BRANCH_SORT_NAME_DESC = 2, /* Descending alphabetical order */
} qgit_branch_sort_t;

BEGIN_DECLS

/**
 * Create a new branch pointing at a target commit.
 *
 * A new direct reference will be created pointing to
 * this target commit. If `force` is true and a reference
 * already exists with the given name, it will be replaced.
 *
 * Only local branches are created through this function.
 *
 * @param out pointer to the newly created reference
 * @param repo repository where to store the branch
 * @param branch_name name for the branch
 * @param target object id to which this branch should point
 * @param force overwrite existing branch
 * @return 0 on success, -1 on error and set errno
 */
QGIT_EXTERN(int)
qgit_branch_create(qgit_reference **out, qgit_repository *repo,
                   const char *branch_name, const qgit_oid *target, int force);

/**
 * Delete an existing branch reference.
 *
 * @param repo repository where the branch lives
 * @param branch_name name of the branch to be deleted
 * @param branch_type type of the considered branch, either
 *		QGIT_BRANCH_LOCAL or QGIT_BRANCH_REMOTE
 * @return 0 on success, -1 on error and set errno
 */
QGIT_EXTERN(int)
qgit_branch_delete(qgit_repository *repo, const char *branch_name,
                   qgit_branch_t branch_type);

/**
 * Move/rename an existing branch reference.
 *
 * @param repo repository where the branch lives
 * @param old_name current name of the branch to be moved
 * @param new_name target name of the branch once the move is performed
 * @param force overwrite an existing branch
 * @return 0 on success, -1 on error and set errno
 */
QGIT_EXTERN(int)
qgit_branch_move(qgit_repository *repo, const char *old_name,
                 const char *new_name, int force);

/**
 * Look up a branch by short name.
 *
 * @param out pointer to the looked-up reference
 * @param repo repository where to find the branch
 * @param branch_name short name of the branch (e.g. "main")
 * @param branch_type type of the considered branch
 * @return 0 on success, -1 on error and set errno
 */
QGIT_EXTERN(int)
qgit_branch_lookup(qgit_reference **out, qgit_repository *repo,
                   const char *branch_name, qgit_branch_t branch_type);

/**
 * Fill a vector with the short names of all branches of the given type.
 *
 * The vector is initialised by this function. Each element is a
 * heap allocated char pointer owned by the vector. The caller must
 * call vec_fini() when done.
 *
 * @param out vector where branch names will be stored
 * @param repo repository where to find the branches
 * @param branch_type filtering flags for the branch listing
 * @param sort sort order for the branch names
 * @return 0 on success, -1 on error and set errno
 */
QGIT_EXTERN(int)
qgit_branch_list(struct vector *out, qgit_repository *repo,
                 qgit_branch_t branch_type, qgit_branch_sort_t sort);

/**
 * Return the short branch name from a reference.
 *
 * Strips the "refs/heads/" or "refs/remotes/" prefix.
 *
 * @param ref the branch reference
 * @return the short branch name, or NULL if ref is not a branch reference
 */
QGIT_EXTERN(const char *) qgit_branch_name(const qgit_reference *ref);

/**
 * Check if a branch reference is the currently checked out HEAD.
 *
 * @param ref the branch reference
 * @return 1 if the branch is HEAD, 0 if it is not, -1 on error and set errno
 */
QGIT_EXTERN(int) qgit_branch_is_head(const qgit_reference *ref);

END_DECLS

#endif
