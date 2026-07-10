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

#include "libqgit/object/commit.h"
#include "revwalk.h"

#include <assert.h>

static int next_commit(qgit_oid *oid, qgit_revwalk *walk)
{
    qgit_commit *commit;

    if (heap_pop(walk->pq, &commit) < 0)
        return -1;

    if (qgit_commit_parentcount(commit)) {
        unsigned int parentcount = walk->mode == QGIT_REVWALK_MODE_ALL
                                       ? qgit_commit_parentcount(commit)
                                       : 1;
        for (unsigned int i = 0; i < parentcount; i++) {
            if (!set_contains(walk->visited,
                              (void *)qgit_commit_parent_oid(commit, i))) {
                qgit_oid *parent_oid;
                qgit_commit *parent;

                parent_oid = calloc(1, sizeof(qgit_oid));
                if (!parent_oid) {
                    qgit_commit_free(commit);
                    return -1;
                }
                qgit_oid_cpy(parent_oid, qgit_commit_parent_oid(commit, i));

                if (qgit_commit_lookup(&parent, walk->repo, parent_oid) < 0) {
                    free(parent_oid);
                    qgit_commit_free(commit);
                    return -1;
                }

                if (heap_push(walk->pq, &parent) < 0) /* enqueue parents */
                {
                    free(parent_oid);
                    qgit_commit_free(parent);
                    qgit_commit_free(commit);
                    return -1;
                }

                if (set_insert(walk->visited, parent_oid) <
                    0) /* mark parents as visited */
                {
                    free(parent_oid);
                    qgit_commit_free(commit);
                    return -1;
                }
            }
        }
    }
    qgit_oid_cpy(oid, qgit_object_id((qgit_object *)commit));
    qgit_commit_free(commit);
    return 0;
}

int qgit_revwalk_next(qgit_oid *oid, qgit_revwalk *walk)
{
    assert(oid && walk);

    if (heap_empty(walk->pq)) {
        qgit_revwalk_reset(walk);
        return 1;
    }

    return next_commit(oid, walk);
}
