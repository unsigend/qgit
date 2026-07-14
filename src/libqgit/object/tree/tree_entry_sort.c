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

#include "tree.h"

#include <assert.h>
#include <collection/vector.h>
#include <string.h>
#include <sys/stat.h>

/* Git tree order: strcmp on names, but directories compare as if they had a
 * trailing '/'. */
static int tree_entry_cmp(const void *a, const void *b)
{
    const qgit_tree_entry *entry_a = (const qgit_tree_entry *)a;
    const qgit_tree_entry *entry_b = (const qgit_tree_entry *)b;
    const char *name_a = entry_a->path;
    const char *name_b = entry_b->path;
    size_t len_a = strlen(name_a);
    size_t len_b = strlen(name_b);
    size_t len = len_a < len_b ? len_a : len_b;
    int cmp;
    unsigned char c1, c2;

    cmp = memcmp(name_a, name_b, len);
    if (cmp)
        return cmp;

    c1 = (unsigned char)name_a[len];
    c2 = (unsigned char)name_b[len];
    if (!c1 && S_ISDIR(entry_a->mode))
        c1 = '/';
    if (!c2 && S_ISDIR(entry_b->mode))
        c2 = '/';

    return (c1 < c2) ? -1 : (c1 > c2) ? 1 : 0;
}

void tree_entry_sort(struct vector *entries)
{
    assert(entries);
    vec_sort(entries, tree_entry_cmp);
}
