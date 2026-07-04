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

#include "loose_backend.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

int qgit_odb_backend_loose(qgit_odb_backend **backend_out,
                           const char *objects_dir)
{
    assert(backend_out && objects_dir);

    struct qgit_loose_backend *backend =
        malloc(sizeof(struct qgit_loose_backend));
    if (!backend)
        return -1;
    memset(backend, 0, sizeof(struct qgit_loose_backend));

    backend->backend.read = qgit_loose_backend_read;
    backend->backend.read_prefix = qgit_loose_backend_read_prefix;
    backend->backend.read_header = qgit_loose_backend_read_header;
    backend->backend.write = qgit_loose_backend_write;
    backend->backend.exists = qgit_loose_backend_exists;
    backend->backend.free = qgit_loose_backend_free;

    backend->objects_dir = strdup(objects_dir);
    if (!backend->objects_dir) {
        qgit_loose_backend_free(&backend->backend);
        return -1;
    }

    *backend_out = &backend->backend;
    return 0;
}