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
#include <stdlib.h>

int qgit_revwalk_new(qgit_revwalk **out, qgit_repository *repo)
{
    assert(out && repo);
    *out = NULL;

    qgit_revwalk *walker = calloc(1, sizeof(qgit_revwalk));
    if (!walker)
        return -1;

    walker->repo = repo;
    walker->mode = QGIT_REVWALK_MODE_ALL;
    walker->sort_mode = QGIT_REVWALK_SORT_TIME; /* default sorting mode */

    if (heap_init(&walker->pq, sizeof(qgit_commit *), pq_commit_cmp_time,
                  pq_commit_free) < 0) {
        qgit_revwalk_free(walker);
        return -1;
    }

    if (set_init(&walker->visited, set_oid_hash, set_oid_cmp, set_oid_free) <
        0) {
        qgit_revwalk_free(walker);
        return -1;
    }

    *out = walker;

    return 0;
}
