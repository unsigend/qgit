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

int qgit_odb_add_backend(qgit_odb *odb, qgit_odb_backend *backend, int priority)
{
    assert(odb && backend);

    struct backend_entry entry = {
        .backend = backend,
        .priority = priority,
    };

    int push = 0;

    for (size_t i = 0; i < vec_size(odb->backends); i++) {
        if (priority >
            ((struct backend_entry *)vec_at(odb->backends, i))->priority) {
            if (vec_insert(odb->backends, i, &entry) < 0)
                return -1;
            push = 1;
            break;
        }
    }

    if (!push) {
        if (vec_pushback(odb->backends, &entry) < 0)
            return -1;
    }

    backend->odb = odb;

    return 0;
}
