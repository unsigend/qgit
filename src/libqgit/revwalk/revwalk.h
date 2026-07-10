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

#endif