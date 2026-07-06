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

#ifndef OBJECT_H
#define OBJECT_H

#include <assert.h>
#include <libqgit/db/odb.h>
#include <libqgit/oid.h>
#include <stdlib.h>

struct qgit_object {
    qgit_oid id;
    qgit_obj_type type;
    qgit_repository *repo;
};

/**
 * Create a qgit_object from a qgit_odb_object.
 *
 * @param out output pointer to receive the object handle, must not be NULL
 * @param odb_object ODB object to create from, must not be NULL
 * @param repo repository to associate with the object, must not be NULL
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_INLINE(int)
qgit_obj_from_odb_obj(qgit_object **out, qgit_odb_object *odb_obj,
                      qgit_repository *repo)
{
    assert(out && odb_obj);
    *out = NULL;

    qgit_object *object = calloc(1, sizeof(struct qgit_object));
    if (!object)
        return -1;

    qgit_oid_cpy(&object->id, qgit_odb_object_id(odb_obj));
    object->type = qgit_odb_object_type(odb_obj);
    object->repo = repo;

    *out = object;

    return 0;
}

#endif