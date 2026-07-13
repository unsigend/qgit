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
#include <libqgit/repo/index.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

int qgit_index_add2(qgit_index *index, const qgit_index_entry *source_entry)
{
    assert(index && source_entry);
    int position = qgit_index_find2(index, source_entry->path,
                                    qgit_index_entry_stage(source_entry));

    if (position != -1) /* replace */
    {
        qgit_index_entry *entry =
            (qgit_index_entry *)vec_at(index->entries, position);
        char *path = strdup(source_entry->path);
        if (!path)
            return -1;
        free(entry->path);
        memcpy(entry, source_entry, sizeof(qgit_index_entry));
        entry->path = path;

    } else /* append a deep copy at end */
    {
        if (vec_pushback(index->entries, (void *)source_entry) < 0)
            return -1;
        qgit_index_entry *last = (qgit_index_entry *)vec_back(index->entries);
        last->path = NULL;
        last->path = strdup(source_entry->path);
        if (!last->path) {
            vec_popback(index->entries, NULL);
            return -1;
        }
    }

    return 0;
}
