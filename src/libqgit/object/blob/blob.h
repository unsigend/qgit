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

#ifndef BLOB_H
#define BLOB_H

#include "../object/object.h"

struct qgit_blob {
    qgit_object object;
    void *data;
    size_t len;
};

/**
 * Parse a blob object from an ODB object.
 *
 * @param out output pointer to receive the blob object, must not be NULL
 * @param odb_obj ODB object to parse from, must not be NULL
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_INTERNAL(int)
blob_parse(qgit_blob *out, qgit_odb_object *odb_obj);

/**
 * Free a blob object.
 *
 * @param blob blob object to free
 */
QGIT_INTERNAL(void) blob_free(qgit_blob *blob);

#endif