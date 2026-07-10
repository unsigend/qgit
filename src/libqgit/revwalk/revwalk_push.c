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

#include "revwalk.h"

#include <assert.h>

int qgit_revwalk_push(qgit_revwalk *walk, const qgit_oid *oid)
{
    assert(walk && oid);

    qgit_object *obj;
    qgit_commit *commit;
    qgit_oid *visited_oid;

    if (!heap_empty(walk->pq) ||
        !set_empty(walk->visited)) /* can't push more than one commit */
        return -1;

    if (qgit_object_lookup(&obj, walk->repo, oid, QGIT_OBJ_COMMIT) < 0)
        return -1;

    commit = (qgit_commit *)obj;
    if (heap_push(walk->pq, &commit) < 0) {
        qgit_object_free(obj);
        return -1;
    }

    visited_oid = calloc(1, sizeof(qgit_oid));
    if (!visited_oid) {
        qgit_revwalk_reset(walk);
        return -1;
    }
    qgit_oid_cpy(visited_oid, oid);

    if (set_insert(walk->visited, visited_oid) < 0) {
        free(visited_oid);
        heap_pop(walk->pq, NULL);
        return -1;
    }

    return 0;
}
