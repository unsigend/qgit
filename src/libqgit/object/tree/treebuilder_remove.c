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

int qgit_treebuilder_remove(qgit_treebuilder *builder, const char *filename)
{
    assert(builder && filename);

    for (unsigned int i = 0; i < vec_size(builder->entries); i++) {
        qgit_tree_entry *entry = vec_at(builder->entries, i);
        if (strcmp(entry->path, filename) == 0) {
            if (vec_remove(builder->entries, i, NULL) < 0)
                return -1;
            return 0;
        }
    }
    return -1;
}
