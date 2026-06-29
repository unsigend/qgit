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
#include <libqgit/db/odb_backend.h>

int qgit_odb_exists(qgit_odb *odb, const qgit_oid *id)
{
    assert(odb && id);

    for (size_t i = 0; i < vec_size(&odb->backends); i++) {
        struct backend_entry *backend =
            (struct backend_entry *)vec_at(&odb->backends, i);
        if (!backend->backend->exists)
            continue;
        int result = backend->backend->exists(backend->backend, id);
        if (result == -1)
            return -1;
        if (result == 1)
            return 1;
    }
    return 0;
}