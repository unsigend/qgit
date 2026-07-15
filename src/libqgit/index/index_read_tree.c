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
#include <errno.h>
#include <libqgit/object/tree.h>
#include <libqgit/repo/index.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FAIL 1

static int tree_walk_cb(const char *root, qgit_tree_entry *entry, void *payload)
{

    qgit_index *index = (qgit_index *)payload;
    qgit_index_entry index_entry = {0};

    char fullpath[PATH_MAX];
    size_t pathlen;

    if (qgit_tree_entry_type(entry) == QGIT_OBJ_TREE) /* skip trees */
        return 0;

    if (snprintf(fullpath, PATH_MAX, "%s%s%s", root, root[0] ? "/" : "",
                 qgit_tree_entry_name(entry)) >= PATH_MAX) {
        errno = ENAMETOOLONG;
        return FAIL;
    }

    pathlen = strlen(fullpath);
    index_entry.path = strdup(fullpath);
    if (!index_entry.path)
        return FAIL;
    index_entry.mode = qgit_tree_entry_attributes(entry);
    index_entry.flags =
        (QGIT_IDXENTRY_STAGE_NORMAL << QGIT_IDXENTRY_STAGESHIFT) |
        (pathlen < QGIT_IDXENTRY_NAMEMASK ? pathlen : QGIT_IDXENTRY_NAMEMASK);
    index_entry.oid = *qgit_tree_entry_id(entry);

    if (vec_pushback(index->entries, &index_entry) < 0) {
        free(index_entry.path);
        return FAIL;
    }

    return 0;
}

int qgit_index_read_tree(qgit_index *index, qgit_tree *tree)
{
    assert(index && tree);

    qgit_index tmp = {0};

    if (vec_init(&tmp.entries, sizeof(qgit_index_entry),
                 qgit_index_entry_free) < 0)
        return -1;

    if (qgit_tree_walk(tree, tree_walk_cb, QGIT_TREEWALK_PRE, &tmp) < 0) {
        vec_free(tmp.entries);
        return -1;
    }

    qgit_index_clear(index);
    vec_free(index->entries);
    index->entries = tmp.entries;

    return 0;
}
