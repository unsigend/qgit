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

#include "index.h"

#include <assert.h>
#include <collection/vector.h>
#include <libqgit/repo/index.h>
#include <string.h>

static int index_entry_cmp(const void *a, const void *b)
{
    const qgit_index_entry *entry_a = (const qgit_index_entry *)a;
    const qgit_index_entry *entry_b = (const qgit_index_entry *)b;

    int n = strcmp(entry_a->path, entry_b->path);
    if (n != 0)
        return n;
    return qgit_index_entry_stage(entry_a) - qgit_index_entry_stage(entry_b);
}

void qgit_index_sort(qgit_index *index)
{
    assert(index);

    vec_sort(index->entries, index_entry_cmp);
}