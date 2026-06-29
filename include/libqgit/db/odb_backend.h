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

#ifndef LIBQGIT_DB_ODB_BACKEND_H
#define LIBQGIT_DB_ODB_BACKEND_H

#include <libqgit/common.h>
#include <libqgit/db/oid.h>
#include <libqgit/types.h>
#include <stddef.h>

BEGIN_DECLS

/* A backend is a vtable that defines how objects are physically stored.
   The odb field is set automatically by qgit_odb_add_backend.
 */
struct qgit_odb_backend {
    qgit_odb *odb; /* owning odb, set by qgit_odb_add_backend */

    /* Read a full object. Caller must free *data_p. */
    int (*read)(void **data_p, size_t *len_p, qgit_obj_type *type_p,
                struct qgit_odb_backend *, const qgit_oid *);

    /* Read only the type and size of an object, without reading its data. */
    int (*read_header)(size_t *len_p, qgit_obj_type *type_p,
                       struct qgit_odb_backend *, const qgit_oid *);

    /* Write an object. Stores the resulting OID in the first argument. */
    int (*write)(qgit_oid *, struct qgit_odb_backend *, const void *, size_t,
                 qgit_obj_type);

    /* Return 1 if the object exists, 0 otherwise. Return -1 on error. */
    int (*exists)(struct qgit_odb_backend *, const qgit_oid *);

    /* Free all resources held by this backend. */
    void (*free)(struct qgit_odb_backend *);
};

/* Create the built-in loose object backend for the given objects directory. */
QGIT_EXTERN(int)
qgit_odb_backend_loose(qgit_odb_backend **out, const char *objects_dir);

END_DECLS

#endif
