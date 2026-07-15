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

#define LOOSE_BACKEND_PRIORITY 2
#define PACK_BACKEND_PRIORITY 1

int qgit_odb_open(qgit_odb **out, const char *objects_dir)
{
    assert(out && objects_dir);

    qgit_odb *odb;
    qgit_odb_backend *loose_backend;

    if (qgit_odb_backend_loose(&loose_backend, objects_dir) < 0)
        return -1;

    if (qgit_odb_new(&odb) < 0) {
        loose_backend->free(loose_backend);
        return -1;
    }

    if (qgit_odb_add_backend(odb, loose_backend, LOOSE_BACKEND_PRIORITY) < 0) {
        loose_backend->free(loose_backend);
        qgit_odb_free(odb);
        return -1;
    }

    *out = odb;

    return 0;
}
