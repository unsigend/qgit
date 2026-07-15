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

#ifndef LIBQGIT_REVWALK_H
#define LIBQGIT_REVWALK_H

#include <libqgit/oid.h>

QGIT_BEGIN_DECLS

/**
 * Emission order for commits yielded by qgit_revwalk_next.
 *
 * Orthogonal to qgit_revwalk_mode (which parents to follow).
 * qgit currently supports time order only.
 */
enum qgit_revwalk_sort_mode {
    QGIT_REVWALK_SORT_TIME = 0u, /* descending committer timestamp (default) */
};

/**
 * Parent-following policy while walking the commit graph.
 *
 * Orthogonal to qgit_revwalk_sort_mode.
 */
enum qgit_revwalk_mode {
    QGIT_REVWALK_MODE_ALL = 0u,          /* follow every parent (default) */
    QGIT_REVWALK_MODE_FIRST_PARENT = 1u, /* follow parent[0] only */
};

/**
 * Allocate a new revision walker for the given repository.
 *
 * The walker may be reset and reused across multiple walks. A walker
 * must not be shared between threads.
 *
 * Defaults: QGIT_REVWALK_MODE_ALL and QGIT_REVWALK_SORT_TIME.
 *
 * @param out  output pointer to receive the walker handle, must not be NULL
 * @param repo repository to walk, must not be NULL
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int) qgit_revwalk_new(qgit_revwalk **out, qgit_repository *repo);

/**
 * Reset a revision walker to its initial blank state.
 *
 * Clears the start commit and all walk progress. Sort and mode settings
 * are preserved. The walker is automatically reset when a walk completes.
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
 * Set the emission order for the walk.
 *
 * Changing the sort mode resets the walker. Only
 * QGIT_REVWALK_SORT_TIME is supported.
 *
 * @param walk      walker to configure, must not be NULL
 * @param sort_mode value from enum qgit_revwalk_sort_mode
 */
QGIT_EXTERN(void)
qgit_revwalk_set_sort(qgit_revwalk *walk,
                      enum qgit_revwalk_sort_mode sort_mode);

/**
 * Set which parents are followed during the walk.
 *
 * QGIT_REVWALK_MODE_ALL walks the full commit topology.
 * QGIT_REVWALK_MODE_FIRST_PARENT matches git log --first-parent.
 * Must be set before the walk starts.
 *
 * @param walk walker to configure, must not be NULL
 * @param mode value from enum qgit_revwalk_mode
 */
QGIT_EXTERN(void)
qgit_revwalk_set_mode(qgit_revwalk *walk, enum qgit_revwalk_mode mode);

/**
 * Set the single starting commit for the walk.
 *
 * qgit walks from exactly one root. The walker must be blank: call only after
 * qgit_revwalk_new or qgit_revwalk_reset, and before qgit_revwalk_next. A
 * second push without reset fails.
 *
 * @param walk walker to configure, must not be NULL
 * @param oid  OID of the commit to start from, must not be NULL
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int) qgit_revwalk_push(qgit_revwalk *walk, const qgit_oid *oid);

/**
 * Set HEAD's commit as the single starting point.
 *
 * @param walk walker to configure, must not be NULL
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int) qgit_revwalk_push_head(qgit_revwalk *walk);

/**
 * Yield the next commit OID from the walk.
 *
 * Requires a start commit from qgit_revwalk_push or qgit_revwalk_push_head.
 * Commits are returned in the order set by qgit_revwalk_set_sort.
 * Returns 1 when the walk is exhausted, at which point the walker is
 * automatically reset.
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
