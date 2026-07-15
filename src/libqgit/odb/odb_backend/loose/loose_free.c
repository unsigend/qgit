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

#include <stdlib.h>

void qgit_loose_backend_free(struct qgit_odb_backend *backend)
{
    if (!backend)
        return;

    struct qgit_loose_backend *loose_backend =
        (struct qgit_loose_backend *)backend;
    if (loose_backend->objects_dir)
        free(loose_backend->objects_dir);
    free(backend);
}
