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
#include <collection/vector.h>
#include <libqgit/db/odb.h>
#include <libqgit/db/odb_backend.h>
#include <libqgit/error.h>
#include <stdlib.h>
#include <string.h>

int qgit_odb_read(qgit_odb_object **out, qgit_odb *odb, const qgit_oid *oid)
{
    assert(out && odb && oid);

    qgit_odb_object *object;
    int found = 0;

    object = calloc(1, sizeof(qgit_odb_object));
    if (!object)
        return -1;

    for (size_t i = 0; i < vec_size(odb->backends); i++) {
        struct backend_entry *entry = vec_at(odb->backends, i);
        if (!entry->backend->read)
            continue;
        qgit_clear_error();
        int ret =
            entry->backend->read(&object->rawobj.data, &object->rawobj.len,
                                 &object->rawobj.type, entry->backend, oid);

        if (ret == 0) {
            found = 1;
            break;
        }
        if (ret == -1) {
            if (qgit_error() == QGITERR_OBJNOTFOUND) {
                memset(object, 0, sizeof(qgit_odb_object));
                continue;
            }
            qgit_odb_object_free(object);
            return -1;
        }
    }

    if (found) {
        qgit_oid_cpy(&object->id, oid);
        *out = object;
        return 0;
    }

    qgit_seterror(QGITERR_OBJNOTFOUND);
    qgit_odb_object_free(object);

    return -1;
}
