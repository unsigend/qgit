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

#ifndef REVWALK_H
#define REVWALK_H

#include <collection/heap.h>
#include <collection/set.h>
#include <libqgit/object/commit.h>
#include <libqgit/revwalk.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct qgit_revwalk {
    qgit_repository *repo;
    enum qgit_revwalk_mode mode;
    enum qgit_revwalk_sort_mode sort_mode;
    struct heap *pq;     /* priority queue */
    struct set *visited; /* visited commits oid */
};

/* priority queue store qgit_commit* helper functions: pq_cmp_time,
 * pq_free_commit */
static int pq_commit_cmp_time(void *a, void *b)
{
    qgit_commit *ca = *(qgit_commit **)a;
    qgit_commit *cb = *(qgit_commit **)b;

    time_t tca = qgit_commit_time(ca);
    time_t tcb = qgit_commit_time(cb);

    if (tca == tcb)
        return 0;
    return tcb > tca ? -1 : 1;
}

static void pq_commit_free(void *data)
{
    if (!data)
        return;
    qgit_commit *commit = *(qgit_commit **)data;
    qgit_commit_free(commit);
}

/* set store qgit_oid* for visited commits. helper functions:
 * set_oid_hash, set_oid_cmp, set_oid_free */
static uint32_t set_oid_hash(void *data)
{
    uint32_t hash = 0;
    qgit_oid *oid = (qgit_oid *)data;
    memcpy(&hash, oid->id, sizeof(uint32_t));
    return hash;
}

static int set_oid_cmp(void *a, void *b)
{
    qgit_oid *oa = (qgit_oid *)a;
    qgit_oid *ob = (qgit_oid *)b;

    return qgit_oid_cmp(oa, ob);
}

static void set_oid_free(void *data)
{
    if (!data)
        return;
    free(data);
}

#endif