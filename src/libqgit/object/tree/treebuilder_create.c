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
#include <libqgit/object/tree.h>
#include <string.h>

int qgit_treebuilder_create(qgit_treebuilder **out, const qgit_tree *source)
{
    assert(out);

    *out = NULL;

    qgit_treebuilder *builder = calloc(1, sizeof(qgit_treebuilder));
    if (!builder)
        return -1;

    if (vec_init(&builder->entries, sizeof(qgit_tree_entry), tree_entry_free) <
        0) {
        qgit_treebuilder_free(builder);
        return -1;
    }

    if (source) {
        for (unsigned int i = 0; i < qgit_tree_entrycount((qgit_tree *)source);
             i++) {
            qgit_tree_entry entry;
            const qgit_tree_entry *source_entry =
                qgit_tree_entry_byindex((qgit_tree *)source, i);

            memcpy(&entry, source_entry,
                   sizeof(qgit_tree_entry)); /* deep copy */
            entry.path = strdup(source_entry->path);
            if (!entry.path) {
                qgit_treebuilder_free(builder);
                return -1;
            }
            if (vec_pushback(builder->entries, &entry) < 0) {
                free(entry.path);
                qgit_treebuilder_free(builder);
                return -1;
            }
        }
    }

    *out = builder;

    return 0;
}
