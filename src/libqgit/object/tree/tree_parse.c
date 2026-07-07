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
#include <libqgit/error.h>
#include <libqgit/oid.h>
#include <string.h>

static void tree_entry_free(void *p)
{
    qgit_tree_entry *entry = (qgit_tree_entry *)p;
    if (entry->path)
        free(entry->path);
}

static char *parse_entry(qgit_tree_entry *entry, char *start, char *end)
{
    char *p = start;
    char *endstr;

    errno = 0;
    entry->mode = strtoul(start, &endstr, 8);
    if (errno || endstr == start || *endstr != ' ') {
        if (!errno)
            qgit_seterror(QGITERR_INVKEY);
        return NULL;
    }

    p = endstr + 1;
    if (p >= end) {
        qgit_seterror(QGITERR_BADTREEFILE);
        return NULL;
    }

    entry->path = strndup(p, end - p);
    if (!entry->path)
        return NULL;

    while (p < end && *p != '\0')
        p++;
    if (p >= end) {
        qgit_seterror(QGITERR_BADTREEFILE);
        free(entry->path);
        return NULL;
    }
    p++;
    if (p + QGIT_OID_RAWSZ > end) {
        qgit_seterror(QGITERR_BADTREEFILE);
        free(entry->path);
        return NULL;
    }
    qgit_oid_fromraw(&entry->oid, (unsigned char *)p);
    p += QGIT_OID_RAWSZ;
    return p;
}

int tree_parse(qgit_tree *out, qgit_odb_object *odb_obj)
{
    assert(out && odb_obj);

    char *p = (char *)qgit_odb_object_data(odb_obj);
    char *end = p + qgit_odb_object_size(odb_obj);

    if (vec_init(&out->entries, sizeof(qgit_tree_entry), tree_entry_free) < 0)
        return -1;

    while (p < end) {
        qgit_tree_entry entry;
        p = parse_entry(&entry, p, end);
        if (!p) {
            tree_free(out);
            return -1;
        }
        if (vec_pushback(out->entries, &entry) < 0) {
            free(entry.path);
            tree_free(out);
            return -1;
        }
    }

    return 0;
}
