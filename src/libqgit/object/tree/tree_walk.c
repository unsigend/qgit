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
#include <errno.h>
#include <libqgit/object/tree.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>

/* callback return value macros */
#define SKIP_SUBTREE(v) ((v) < 0)
#define FAILED(v) ((v) > 0)

static int foreach (qgit_tree *tree, qgit_treewalk_cb callback,
                    qgit_treewalk_mode mode, void *payload, const char *root)
{
    qgit_tree_entry *entry;
    int ret;

    for (size_t i = 0; i < vec_size(tree->entries); i++) {
        entry = vec_at(tree->entries, i);
        if (qgit_tree_entry_type(entry) == QGIT_OBJ_TREE) {

            char subpath[PATH_MAX];
            qgit_tree *subtree;

            if (snprintf(subpath, PATH_MAX, "%s%s%s", root, root[0] ? "/" : "",
                         qgit_tree_entry_name(entry)) >= PATH_MAX) {
                errno = ENAMETOOLONG;
                return -1;
            }

            if (mode == QGIT_TREEWALK_PRE) /* pre-order traversal */
            {
                ret = callback(root, entry, payload);
                if (FAILED(ret))
                    return -1;

                if (!SKIP_SUBTREE(ret)) {
                    if (qgit_tree_lookup(&subtree, tree->object.repo,
                                         qgit_tree_entry_id(entry)) < 0)
                        return -1;

                    ret = foreach (subtree, callback, mode, payload, subpath);
                    qgit_tree_free(subtree);

                    if (ret == -1)
                        return ret;
                }

            } else if (mode == QGIT_TREEWALK_POST) /* post-order traversal */
            {
                if (qgit_tree_lookup(&subtree, tree->object.repo,
                                     qgit_tree_entry_id(entry)) < 0)
                    return -1;

                ret = foreach (subtree, callback, mode, payload, subpath);
                qgit_tree_free(subtree);

                if (ret == -1)
                    return ret;

                ret = callback(root, entry, payload);
                if (FAILED(ret))
                    return -1;

            } else
                return -1;

        } else if (qgit_tree_entry_type(entry) == QGIT_OBJ_BLOB) {
            ret = callback(root, entry, payload);
            if (FAILED(ret))
                return -1;
        } else
            return -1;
    }

    return 0;
}

int qgit_tree_walk(qgit_tree *tree, qgit_treewalk_cb callback,
                   qgit_treewalk_mode mode, void *payload)
{
    assert(tree && callback);

    return foreach (tree, callback, mode, payload, "");
}
