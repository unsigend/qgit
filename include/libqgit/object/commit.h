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

#ifndef LIBQGIT_OBJECT_COMMIT_H
#define LIBQGIT_OBJECT_COMMIT_H

#include <libqgit/common.h>
#include <libqgit/object/object.h>
#include <libqgit/object/signature.h>
#include <libqgit/object/tree.h>
#include <libqgit/oid.h>
#include <libqgit/types.h>
#include <time.h>

QGIT_BEGIN_DECLS

/**
 * Look up a commit object in a repository by OID.
 *
 * @param out  output pointer to receive the commit handle, must not be NULL
 * @param repo repository to search, must not be NULL
 * @param id   OID of the commit, must not be NULL
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_INLINE(int)
qgit_commit_lookup(qgit_commit **out, qgit_repository *repo, const qgit_oid *id)
{
    return qgit_object_lookup((qgit_object **)out, repo, id, QGIT_OBJ_COMMIT);
}

/**
 * Look up a commit object in a repository by abbreviated OID prefix.
 *
 * @param out  output pointer to receive the commit handle, must not be NULL
 * @param repo repository to search, must not be NULL
 * @param id   partial OID with the unused suffix zeroed
 * @param len  number of hex characters in the prefix
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_INLINE(int)
qgit_commit_lookup_prefix(qgit_commit **out, qgit_repository *repo,
                          const qgit_oid *id, unsigned int len)
{
    return qgit_object_lookup_prefix((qgit_object **)out, repo, id, len,
                                     QGIT_OBJ_COMMIT);
}

/**
 * Free a commit handle.
 *
 * @param commit commit to free, no-op if NULL
 */
QGIT_INLINE(void) qgit_commit_free(qgit_commit *commit)
{
    qgit_object_free((qgit_object *)commit);
}

/**
 * Return the OID of a commit.
 *
 * @param commit commit to query, must not be NULL
 * @return pointer to the commit OID, owned by the commit
 */
QGIT_EXTERN(const qgit_oid *) qgit_commit_id(qgit_commit *commit);

/**
 * Return the full commit message.
 *
 * The returned pointer is owned by the commit and is valid until
 * qgit_commit_free is called. qgit always encodes messages as UTF-8.
 *
 * @param commit commit to query, must not be NULL
 * @return null-terminated commit message
 */
QGIT_EXTERN(const char *) qgit_commit_message(qgit_commit *commit);

/**
 * Return the commit timestamp (committer time) as seconds since the Unix epoch.
 *
 * @param commit commit to query, must not be NULL
 * @return commit time
 */
QGIT_EXTERN(time_t) qgit_commit_time(qgit_commit *commit);

/**
 * Return the committer timezone offset in minutes east of UTC.
 *
 * @param commit commit to query, must not be NULL
 * @return timezone offset in minutes
 */
QGIT_EXTERN(int) qgit_commit_time_offset(qgit_commit *commit);

/**
 * Return the committer signature of a commit.
 *
 * The returned pointer is owned by the commit and is valid until
 * qgit_commit_free is called.
 *
 * @param commit commit to query, must not be NULL
 * @return pointer to the committer signature
 */
QGIT_EXTERN(const qgit_signature *) qgit_commit_committer(qgit_commit *commit);

/**
 * Return the author signature of a commit.
 *
 * The returned pointer is owned by the commit and is valid until
 * qgit_commit_free is called.
 *
 * @param commit commit to query, must not be NULL
 * @return pointer to the author signature
 */
QGIT_EXTERN(const qgit_signature *) qgit_commit_author(qgit_commit *commit);

/**
 * Load the tree object a commit points to.
 *
 * The returned tree must be freed with qgit_tree_free.
 *
 * @param out    output pointer to receive the tree handle, must not be NULL
 * @param commit commit to query, must not be NULL
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int) qgit_commit_tree(qgit_tree **out, qgit_commit *commit);

/**
 * Return the OID of the tree a commit points to without loading it.
 *
 * @param commit commit to query, must not be NULL
 * @return pointer to the tree OID, owned by the commit
 */
QGIT_EXTERN(const qgit_oid *) qgit_commit_tree_oid(qgit_commit *commit);

/**
 * Return the number of parents of a commit.
 *
 * @param commit commit to query, must not be NULL
 * @return parent count (0 for a root commit)
 */
QGIT_EXTERN(unsigned int) qgit_commit_parentcount(qgit_commit *commit);

/**
 * Load the nth parent of a commit.
 *
 * The returned commit must be freed with qgit_commit_free.
 *
 * @param out    output pointer to receive the parent commit, must not be NULL
 * @param commit commit to query, must not be NULL
 * @param n      zero-based parent index
 * @return 0 on success, -1 if n is out of range or on error and sets errno
 */
QGIT_EXTERN(int)
qgit_commit_parent(qgit_commit **out, qgit_commit *commit, unsigned int n);

/**
 * Return the OID of the nth parent of a commit without loading it.
 *
 * @param commit commit to query, must not be NULL
 * @param n      zero-based parent index
 * @return pointer to the parent OID, or NULL if n is out of range
 */
QGIT_EXTERN(const qgit_oid *)
qgit_commit_parent_oid(qgit_commit *commit, unsigned int n);

/**
 * Write a new commit object to the repository.
 *
 * The commit message is stored as UTF-8. If update_ref is non-NULL the
 * named reference is updated (or created) to point to the new commit
 * after it is written, use "HEAD" to advance the current branch.
 *
 * @param oid          output pointer to receive the new commit OID, must not be
 *                     NULL
 * @param repo         repository to write into, must not be NULL
 * @param update_ref   reference to update after writing, or NULL to skip
 * @param author       author signature, must not be NULL
 * @param committer    committer signature, must not be NULL
 * @param message      commit message, must not be NULL
 * @param tree         tree the commit records, must not be NULL
 * @param parent_count number of parent commits (0 for a root commit)
 * @param parents      array of parent_count commit pointers, or NULL when
 *                     parent_count is 0
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int)
qgit_commit_create(qgit_oid *oid, qgit_repository *repo, const char *update_ref,
                   const qgit_signature *author,
                   const qgit_signature *committer, const char *message,
                   const qgit_tree *tree, int parent_count,
                   const qgit_commit *parents[]);

QGIT_END_DECLS

#endif
