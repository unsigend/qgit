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
#include <fs.h>
#include <limits.h>
#include <stdio.h>

int loose_backend_exists(struct qgit_odb_backend *backend, const qgit_oid *oid)
{
    assert(backend && oid);

    struct loose_backend *loose_backend = (struct loose_backend *)backend;
    char path[PATH_MAX];

    if (loose_oid_path(loose_backend->objects_dir, oid, path, PATH_MAX) == -1)
        return -1;

    return file_exists(path);
}
