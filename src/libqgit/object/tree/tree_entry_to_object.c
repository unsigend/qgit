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
#include <libqgit/object/object.h>
#include <sys/stat.h>

int qgit_tree_entry_to_object(qgit_object **out, qgit_repository *repo,
                              const qgit_tree_entry *entry)
{
    assert(out && repo && entry);
    *out = NULL;

    qgit_object *obj;
    qgit_obj_type type;

    if (S_ISDIR(entry->mode))
        type = QGIT_OBJ_TREE;
    else if (S_ISREG(entry->mode) || S_ISLNK(entry->mode))
        type = QGIT_OBJ_BLOB;
    else {
        qgit_seterror(QGITERR_BADTREEFILE);
        return -1;
    }

    if (qgit_object_lookup(&obj, repo, &entry->oid, type) < 0)
        return -1;

    *out = obj;

    return 0;
}
