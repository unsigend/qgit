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

#include "odb.h"

#include <assert.h>
#include <libqgit/db/odb.h>
#include <libqgit/error.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

int qgit_odb_read_prefix(qgit_odb_object **out, qgit_odb *odb,
                         const qgit_oid *short_id, unsigned int len)
{
    assert(out && odb && short_id && len >= QGIT_OID_MINPREFIXLEN &&
           len <= QGIT_OID_RAWSZ * 2);

    *out = NULL;

    int found = 0;
    qgit_odb_object *object = calloc(1, sizeof(qgit_odb_object));
    if (!object)
        return -1;

    for (size_t i = 0; i < vec_size(odb->backends); i++) {
        struct backend_entry *backend =
            (struct backend_entry *)vec_at(odb->backends, i);
        if (!backend->backend->read_prefix)
            continue;
        int result = backend->backend->read_prefix(
            &object->oid, &object->rawobj.data, &object->rawobj.len,
            &object->rawobj.type, backend->backend, short_id, len);
        if (result == -1) {
            if (qgit_geterrno() == QGITERR_OBJ_NOT_FOUND) {
                memset(object, 0, sizeof(qgit_odb_object));
                continue;
            }
            qgit_odb_object_free(object);
            return -1;
        }
        if (result == 0) {
            found = 1;
            break;
        }
    }

    if (found) {
        *out = object;
        return 0;
    }
    qgit_seterrno(QGITERR_OBJ_NOT_FOUND);
    qgit_odb_object_free(object);
    return -1;
}