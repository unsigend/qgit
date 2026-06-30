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

#include "object.h"

#include <assert.h>
#include <libqgit/db/odb.h>
#include <libqgit/error.h>
#include <libqgit/repository.h>
#include <stdlib.h>

int qgit_object_lookup_prefix(qgit_object **object, qgit_repository *repo,
                              const qgit_oid *id, unsigned int len,
                              qgit_obj_type type)
{
    assert(object && repo && id && len >= QGIT_OID_MINPREFIXLEN &&
           len <= QGIT_OID_RAWSZ * 2);

    *object = NULL;
    struct qgit_odb_object *odb_object;
    struct qgit_odb *odb;

    if (qgit_repository_odb(&odb, repo) == -1 ||
        qgit_odb_read_prefix(&odb_object, odb, id, len) == -1)
        return -1;

    if (type != QGIT_OBJ_ANY && type != qgit_odb_object_type(odb_object)) {
        qgit_seterrno(QGITERR_OBJ_TYPE_MISMATCH);
        qgit_odb_object_free(odb_object);
        return -1;
    }

    *object = calloc(1, sizeof(qgit_object));
    if (!*object) {
        qgit_odb_object_free(odb_object);
        return -1;
    }

    (*object)->owner = repo;
    (*object)->type = qgit_odb_object_type(odb_object);
    qgit_oid_copy(&(*object)->oid, qgit_odb_object_id(odb_object));

    qgit_odb_object_free(odb_object);

    return 0;
}