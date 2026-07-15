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

#include "../../index/index.h"
#include "collection/string.h"
#include "tree.h"

#include <errno.h>
#include <libqgit/object/tree.h>
#include <libqgit/repo/index.h>
#include <limits.h>
#include <string.h>

/* Write one tree for entries under prefix. The parameter cur is a shared index
 * cursor: this call consumes consecutive stage-0 entries whose path starts with
 * prefix, recursively writing child trees for each immediate subdirectory, then
 * writes this tree to the ODB. */
static int write_tree(qgit_oid *oid, qgit_index *index, unsigned int *cur,
                      const char *prefix)
{
    qgit_treebuilder *builder;
    size_t prefix_len = strlen(prefix);

    if (!index->owner) {
        errno = EINVAL;
        return -1;
    }

    if (qgit_treebuilder_create(&builder, NULL) < 0)
        return -1;

    while (*cur < qgit_index_entrycount(index)) {
        qgit_index_entry *index_entry = qgit_index_get(index, *cur);

        if (qgit_index_entry_stage(index_entry) !=
            QGIT_IDXENTRY_STAGE_NORMAL) /* skip non-normal staged entries */
        {
            (*cur)++;
            continue;
        }

        if (!str_startswith(index_entry->path, prefix))
            break;

        const char *rest = index_entry->path + prefix_len;
        const char *slash = strchr(rest, '/');

        if (!slash) /* directly add entry to builder */
        {
            if (qgit_treebuilder_insert(NULL, builder, rest, &index_entry->oid,
                                        index_entry->mode) <
                0) /* rest is a pure file name */
            {
                qgit_treebuilder_free(builder);
                return -1;
            }
            (*cur)++;
        } else /* build subtree */
        {
            char nestprefix[PATH_MAX];
            char dirname[PATH_MAX];
            size_t dirlen = slash - rest;
            qgit_oid subtree_oid;

            if (dirlen + prefix_len + 2 >= PATH_MAX) {
                qgit_treebuilder_free(builder);
                errno = ENAMETOOLONG;
                return -1;
            }

            strncpy(dirname, rest, dirlen);
            dirname[dirlen] = '\0';

            strncpy(nestprefix, prefix, prefix_len);
            strncpy(nestprefix + prefix_len, dirname, dirlen);
            nestprefix[prefix_len + dirlen] = '/';
            nestprefix[prefix_len + dirlen + 1] = '\0';

            if (write_tree(&subtree_oid, index, cur, nestprefix) < 0) {
                qgit_treebuilder_free(builder);
                return -1;
            }

            if (qgit_treebuilder_insert(NULL, builder, dirname, &subtree_oid,
                                        040000) < 0) {
                qgit_treebuilder_free(builder);
                return -1;
            }
        }
    }

    if (qgit_treebuilder_write(oid, index->owner, builder) < 0) {
        qgit_treebuilder_free(builder);
        return -1;
    }

    qgit_treebuilder_free(builder);
    return 0;
}

int qgit_tree_create_fromindex(qgit_oid *oid, qgit_index *index)
{
    assert(oid && index);
    qgit_index_sort(index);

    unsigned int i = 0;

    return write_tree(oid, index, &i, "");
}
