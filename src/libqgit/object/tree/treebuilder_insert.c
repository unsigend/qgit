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

#include <collection/vector.h>
#include <libqgit/object/tree.h>
#include <string.h>

int qgit_treebuilder_insert(qgit_tree_entry **entry_out,
                            qgit_treebuilder *builder, const char *filename,
                            const qgit_oid *id, unsigned int attributes)
{
    assert(builder && filename && id);
    if (entry_out)
        *entry_out = NULL;

    const qgit_tree_entry *existing;

    if ((existing = qgit_treebuilder_get(builder, filename)) !=
        NULL) /* update existing entry */
    {
        qgit_tree_entry *mutate = (qgit_tree_entry *)existing;

        mutate->oid = *id;
        mutate->mode = attributes;

        if (entry_out)
            *entry_out = mutate;
        return 0;

    } else /* pushback new entry */
    {
        qgit_tree_entry entry = {0};

        entry.path = strdup(filename);
        if (!entry.path)
            return -1;

        entry.oid = *id;
        entry.mode = attributes;

        if (vec_pushback(builder->entries, &entry) < 0) {
            free(entry.path);
            return -1;
        }

        if (entry_out)
            *entry_out = vec_back(builder->entries);
        return 0;
    }
}
