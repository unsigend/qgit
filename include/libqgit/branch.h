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
#include <libqgit/refs.h>
#include <libqgit/types.h>

/* Branch type flag. */
typedef enum {
    QGIT_BRANCH_LOCAL = 1,
    QGIT_BRANCH_REMOTE = 2,
    QGIT_BRANCH_ALL = QGIT_BRANCH_LOCAL | QGIT_BRANCH_REMOTE,
} qgit_branch_t;

/* Sort order for qgit_branch_foreach. */
typedef enum {
    QGIT_BRANCH_SORT_NONE = 0,      /* filesystem/readdir order */
    QGIT_BRANCH_SORT_NAME = 1,      /* ascending alphabetical */
    QGIT_BRANCH_SORT_NAME_DESC = 2, /* descending alphabetical */
} qgit_branch_sort_t;

BEGIN_DECLS

/* Create a new branch pointing at the given commit OID. Writes a direct
   reference under refs/heads/. The returned reference must be freed with
   qgit_reference_free(). If force is non-zero, an existing branch with the
   same name is overwritten. Only local branches are created through this
   function. */
QGIT_EXTERN(int)
qgit_branch_create(qgit_reference **out, qgit_repository *repo,
                   const char *branch_name, const qgit_oid *target, int force);

/* Delete a branch by short name (e.g. "main"). */
QGIT_EXTERN(int)
qgit_branch_delete(qgit_repository *repo, const char *branch_name,
                   qgit_branch_t branch_type);

/* Rename a branch. If force is non-zero, overwrites an existing branch with
   new_name. */
QGIT_EXTERN(int)
qgit_branch_move(qgit_repository *repo, const char *old_name,
                 const char *new_name, int force);

/* Look up a branch by short name (e.g. "main"). Returns the underlying
   reference, which must be freed with qgit_reference_free(). */
QGIT_EXTERN(int)
qgit_branch_lookup(qgit_reference **out, qgit_repository *repo,
                   const char *branch_name, qgit_branch_t branch_type);

/* Fill a vector with the short names of all branches of the given type,
   optionally sorted. The vector is initialised by this function, each element
   is a heap-allocated char * owned by the vector. The caller must call
   vec_fini() when done. */
QGIT_EXTERN(int)
qgit_branch_list(struct vector *out, qgit_repository *repo,
                 qgit_branch_t branch_type, qgit_branch_sort_t sort);

/* Return the short branch name from a reference (strips "refs/heads/" or
   "refs/remotes/" prefix). Returns NULL if ref is not a branch reference. */
QGIT_EXTERN(const char *) qgit_branch_name(const qgit_reference *ref);

/* Return 1 if the branch reference is the currently checked-out HEAD,
   0 otherwise. */
QGIT_EXTERN(int) qgit_branch_is_head(const qgit_reference *ref);

END_DECLS

#endif
