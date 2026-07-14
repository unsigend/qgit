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
#include <libqgit/db/odb.h>
#include <libqgit/object/object.h>
#include <libqgit/object/tree.h>
#include <libqgit/repo/repository.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

static int tree_entries_fmt(qgit_treebuilder *builder, char *buf, size_t buflen)
{
    if (vec_size(builder->entries) == 0)
        return 0;

    if (buf && buflen) {
        int total = 0, nwrite;
        char *p = buf;
        for (unsigned int i = 0; i < vec_size(builder->entries); i++) {
            qgit_tree_entry *entry = vec_at(builder->entries, i);
            nwrite =
                snprintf(p, buflen - total, "%o %s", entry->mode, entry->path);
            if (nwrite < 0 || (size_t)nwrite >= buflen - total)
                return -1;
            total += nwrite;
            if (buflen - total < QGIT_OID_RAWSZ + 1)
                return -1;
            p += nwrite;
            p++;
            memcpy(p, entry->oid.id, QGIT_OID_RAWSZ);
            p += QGIT_OID_RAWSZ;
            total += QGIT_OID_RAWSZ + 1;
        }
        return total;
    } else /* dry run */
    {
        int total = 0, nwrite;
        for (unsigned int i = 0; i < vec_size(builder->entries); i++) {
            qgit_tree_entry *entry = vec_at(builder->entries, i);
            nwrite = snprintf(NULL, 0, "%o %s", entry->mode, entry->path);
            if (nwrite < 0)
                return -1;
            total += nwrite;
            total += QGIT_OID_RAWSZ + 1; /* \0<sha1-20> */
        }
        return total;
    }
}

int qgit_treebuilder_write(qgit_oid *oid, qgit_repository *repo,
                           qgit_treebuilder *builder)
{
    assert(oid && repo && builder);

    size_t buflen;
    char *buf;
    int total;

    tree_entry_sort(builder->entries); /* sort entries for tree order */

    if ((total = tree_entries_fmt(builder, NULL, 0)) < 0)
        return -1;
    buflen = (size_t)total;
    if (buflen) {
        buf = malloc(buflen);
        if (!buf)
            return -1;
    } else
        buf = NULL;

    if ((total = tree_entries_fmt(builder, buf, buflen)) < 0) {
        if (buf)
            free(buf);
        return -1;
    }

    if (qgit_odb_write(oid, qgit_repository_odb(repo), buf, buflen,
                       QGIT_OBJ_TREE) < 0) {
        if (buf)
            free(buf);
        return -1;
    }
    if (buf)
        free(buf);

    return 0;
}
