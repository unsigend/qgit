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
#include <libqgit/error.h>
#include <stdlib.h>
#include <string.h>

int qgit_odb_backend_loose(qgit_odb_backend **out, const char *objects_dir)
{
    assert(out && objects_dir);

    *out = NULL;

    struct loose_backend *backend = malloc(sizeof(struct loose_backend));
    QGITERR_CHECK_ALLOC(backend);
    memset(backend, 0, sizeof(struct loose_backend));

    backend->objects_dir = strdup(objects_dir);
    if (!backend->objects_dir) {
        loose_backend_free(&backend->base);
        return -1;
    }

    backend->base.read = loose_backend_read;
    backend->base.read_header = loose_backend_read_header;
    backend->base.write = loose_backend_write;
    backend->base.exists = loose_backend_exists;
    backend->base.free = loose_backend_free;

    *out = &backend->base;
    return 0;
}