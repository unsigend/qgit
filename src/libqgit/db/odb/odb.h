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

#ifndef ODB_H
#define ODB_H

#include <libqgit/db/oid.h>
#include <libqgit/types.h>
#include <stddef.h>

struct qgit_odb_object {
    qgit_oid oid;
    void *data; /* raw decompressed object data */
    size_t len; /* length of the raw object data */
    qgit_obj_type type;
};

#endif