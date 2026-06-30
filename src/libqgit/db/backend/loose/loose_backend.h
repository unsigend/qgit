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

#ifndef LOOSE_BACKEND_H
#define LOOSE_BACKEND_H

#include <libqgit/common.h>
#include <libqgit/db/odb_backend.h>
#include <libqgit/db/oid.h>
#include <stddef.h>

struct loose_backend {
    qgit_odb_backend base;
    char *objects_dir;
};

QGIT_INTERNAL(int)
loose_backend_read(void **data_p, size_t *len_p, qgit_obj_type *type_p,
                   qgit_odb_backend *backend, const qgit_oid *oid);
QGIT_INTERNAL(int)
loose_backend_read_prefix(qgit_oid *full_oid_out, void **data_p, size_t *len_p,
                          qgit_obj_type *type_p, qgit_odb_backend *backend,
                          const qgit_oid *short_id, unsigned int len);
QGIT_INTERNAL(int)
loose_backend_read_header(size_t *len_p, qgit_obj_type *type_p,
                          qgit_odb_backend *backend, const qgit_oid *oid);
QGIT_INTERNAL(int)
loose_backend_write(qgit_oid *out, qgit_odb_backend *backend, const void *data,
                    size_t len, qgit_obj_type type);
QGIT_INTERNAL(int)
loose_backend_exists(qgit_odb_backend *backend, const qgit_oid *oid);

QGIT_INTERNAL(void) loose_backend_free(qgit_odb_backend *backend);

/* Get the path to the object file in the objects directory. Return 0 on
   success, -1 on error. */
QGIT_INTERNAL(int)
loose_oid_path(const char *objects_dir, const qgit_oid *oid, char *path,
               size_t pathlen);

#endif