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

int qgit_odb_write(qgit_oid *out, qgit_odb *odb, const void *data, size_t len,
                   qgit_obj_type type)
{
    assert(out && odb);

    for (size_t i = 0; i < vec_size(odb->backends); i++) {
        struct backend_entry *backend =
            (struct backend_entry *)vec_at(odb->backends, i);
        if (!backend->backend->write)
            continue;
        int result =
            backend->backend->write(out, backend->backend, data, len, type);
        /* If result is -1, try the next writable backend */
        if (result == 0)
            return 0;
    }

    /* No writable backend found */
    return -1;
}