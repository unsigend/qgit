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

#ifndef LIBQGIT_REPO_REVWALK_H
#define LIBQGIT_REPO_REVWALK_H

#include <libqgit/common.h>
#include <libqgit/oid.h>
#include <libqgit/types.h>

QGIT_BEGIN_DECLS

#define QGIT_SORT_NONE 0u /* implementation-defined order (default) */
#define QGIT_SORT_TOPOLOGICAL (1u << 0) /* parents before children */
#define QGIT_SORT_TIME (1u << 1)        /* descending committer timestamp */
#define QGIT_SORT_REVERSE (1u << 2)     /* invert the output order */
#define QGIT_SORT_FIRST_PARENT                                                 \
    (1u << 3) /* follow only the first parent of each commit */

/**
 * Allocate a new revision walker for the given repository.
 *
 * The walker may be reset and reused across multiple walks to amortize
 * allocation cost. A walker must not be shared between threads.
 *
 * @param out  output pointer to receive the walker handle, must not be NULL
 * @param repo repository to walk, must not be NULL
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int) qgit_revwalk_new(qgit_revwalk **out, qgit_repository *repo);

/**
 * Reset a revision walker to its initial blank state.
 *
 * Clears all pushed and hidden commits. The walker is automatically
 * reset when a walk completes.
 *
 * @param walk walker to reset, must not be NULL
 */
QGIT_EXTERN(void) qgit_revwalk_reset(qgit_revwalk *walk);

/**
 * Free a revision walker.
 *
 * @param walk walker to free, no-op if NULL
 */
QGIT_EXTERN(void) qgit_revwalk_free(qgit_revwalk *walk);

/**
 * Set the sort mode for the walk.
 *
 * Must be called before the first qgit_revwalk_next. Changing the sort
 * mode after walking has started resets the walker.
 *
 * @param walk      walker to configure, must not be NULL
 * @param sort_mode bitwise OR of QGIT_SORT_* flags
 */
QGIT_EXTERN(void)
qgit_revwalk_sorting(qgit_revwalk *walk, unsigned int sort_mode);

/**
 * Mark a commit as a starting point for the walk.
 *
 * At least one commit must be pushed before calling qgit_revwalk_next.
 * Multiple pushes add multiple roots.
 *
 * @param walk walker to configure, must not be NULL
 * @param oid  OID of the commit to start from, must not be NULL
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int) qgit_revwalk_push(qgit_revwalk *walk, const qgit_oid *oid);

/**
 * Push the commit that HEAD currently points to as a starting point.
 *
 * @param walk walker to configure, must not be NULL
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int) qgit_revwalk_push_head(qgit_revwalk *walk);

/**
 * Mark a commit (and all its ancestors) as uninteresting.
 *
 * Commits reachable from oid will be excluded from the walk output.
 *
 * @param walk walker to configure, must not be NULL
 * @param oid  OID of the commit to hide, must not be NULL
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int) qgit_revwalk_hide(qgit_revwalk *walk, const qgit_oid *oid);

/**
 * Yield the next commit OID from the walk.
 *
 * Commits are returned in the order determined by the sort mode. Returns
 * 1 when the walk is exhausted (no more commits), at which point the
 * walker is automatically reset.
 *
 * @param oid  output pointer to receive the next commit OID, must not be NULL
 * @param walk walker to advance, must not be NULL
 * @return 0 if a commit was written into oid, 1 if the walk is done,
 *         -1 on error and sets errno
 */
QGIT_EXTERN(int) qgit_revwalk_next(qgit_oid *oid, qgit_revwalk *walk);

/**
 * Return the repository this walker operates on.
 *
 * @param walk walker to query, must not be NULL
 * @return pointer to the owning repository
 */
QGIT_EXTERN(qgit_repository *) qgit_revwalk_repository(qgit_revwalk *walk);

QGIT_END_DECLS

#endif
