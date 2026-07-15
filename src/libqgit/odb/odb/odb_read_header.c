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
#include <libqgit/error.h>

int qgit_odb_read_header(size_t *len_p, qgit_obj_type *type_p, qgit_odb *odb,
                         const qgit_oid *oid)
{
    assert(len_p && type_p && odb && oid);

    for (size_t i = 0; i < vec_size(odb->backends); i++) {
        struct backend_entry *entry = vec_at(odb->backends, i);
        if (!entry->backend->read_header)
            continue;
        qgit_clear_error();
        int ret =
            entry->backend->read_header(len_p, type_p, entry->backend, oid);
        if (ret == 0)
            return 0;
        if (ret == -1) {
            if (qgit_error() == QGITERR_OBJNOTFOUND) {
                *len_p = 0;
                *type_p = QGIT_OBJ_BAD;
                continue;
            }
            return -1;
        }
    }

    qgit_seterror(QGITERR_OBJNOTFOUND);
    return -1;
}
