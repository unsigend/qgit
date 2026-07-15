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

#include <libqgit/error.h>
#include <libqgit/object/tree.h>
#include <string.h>

int qgit_tree_get_subtree(qgit_tree **out, qgit_tree *root,
                          const char *subtree_path)
{
    assert(out && root && subtree_path);
    *out = NULL;

    char *path = strdup(subtree_path);
    if (!path)
        return -1;

    qgit_tree *current = root;
    qgit_tree *next = NULL;
    int error = 0;

    char *component = strtok(path, "/");
    while (component) {
        const qgit_tree_entry *entry =
            qgit_tree_entry_byname(current, component);
        if (!entry || qgit_tree_entry_type(entry) != QGIT_OBJ_TREE) {
            qgit_seterror(QGITERR_NOSUBTREE);
            error = -1;
            break;
        }

        if (qgit_tree_lookup(&next, current->object.repo,
                             qgit_tree_entry_id(entry)) < 0) {
            error = -1;
            break;
        }

        if (current != root)
            qgit_tree_free(current);

        current = next;
        component = strtok(NULL, "/");
    }

    free(path);

    if (error) {
        if (current != root)
            qgit_tree_free(current);
        return -1;
    }

    *out = current;
    return 0;
}
