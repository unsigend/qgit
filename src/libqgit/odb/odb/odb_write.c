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

int qgit_odb_write(qgit_oid *oid, qgit_odb *odb, const void *data, size_t len,
                   qgit_obj_type type)
{
    assert(oid && odb);

    for (size_t i = 0; i < vec_size(odb->backends); i++) {
        struct backend_entry *entry = vec_at(odb->backends, i);
        if (!entry->backend->write)
            continue;
        int ret = entry->backend->write(oid, entry->backend, data, len, type);
        if (ret == 0)
            return 0;
        /* if result is -1 try next writable backend */
    }

    /* No writable backend found */
    return -1;
}
