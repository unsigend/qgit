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

#include "../blob/blob.h"
#include "../commit/commit.h"
#include "../tag/tag.h"
#include "../tree/tree.h"

#include <libqgit/object/object.h>

int qgit_object_from_odb_obj(qgit_object **out, qgit_odb_object *odb_obj,
                             qgit_repository *repo)
{
    assert(out && odb_obj);
    *out = NULL;

    size_t objsz;
    qgit_object *object;
    qgit_obj_type type;

    type = qgit_odb_object_type(odb_obj);

    objsz = qgit_object_mem_size(type);
    if (!objsz)
        return -1;

    object = calloc(1, objsz);
    if (!object)
        return -1;

    qgit_oid_cpy(&object->id, qgit_odb_object_id(odb_obj));
    object->type = type;
    object->repo = repo;

    switch (type) {
    case QGIT_OBJ_COMMIT:
        if (commit_parse((qgit_commit *)object, odb_obj) < 0) {
            qgit_object_free(object);
            return -1;
        }
        break;
    case QGIT_OBJ_TAG:
        if (tag_parse((qgit_tag *)object, odb_obj) < 0) {
            qgit_object_free(object);
            return -1;
        }
        break;
    case QGIT_OBJ_TREE:
        if (tree_parse((qgit_tree *)object, odb_obj) < 0) {
            qgit_object_free(object);
            return -1;
        }
        break;
    case QGIT_OBJ_BLOB:
        if (blob_parse((qgit_blob *)object, odb_obj) < 0) {
            qgit_object_free(object);
            return -1;
        }
        break;
    default:
        qgit_seterror(QGITERR_BADREFTYPE);
        qgit_object_free(object);
        return -1;
    }

    *out = object;

    return 0;
}