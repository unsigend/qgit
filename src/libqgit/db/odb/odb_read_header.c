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

int qgit_odb_read_header(size_t *len_p, qgit_obj_type *type_p, qgit_odb *odb,
                         const qgit_oid *id)
{
    assert(len_p && type_p && odb && id);

    *len_p = 0;
    *type_p = QGIT_OBJ_BAD;
    int found = 0;

    for (size_t i = 0; i < vec_size(odb->backends); i++) {
        struct backend_entry *backend =
            (struct backend_entry *)vec_at(odb->backends, i);
        if (!backend->backend->read_header)
            continue;
        int result =
            backend->backend->read_header(len_p, type_p, backend->backend, id);
        if (result == -1) {
            if (qgit_geterrno() == QGITERR_OBJ_NOT_FOUND)
                continue;
            return -1;
        }
        if (result == 0) {
            found = 1;
            break;
        }
    }

    if (found)
        return 0;
    qgit_seterrno(QGITERR_OBJ_NOT_FOUND);
    return -1;
}