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

#ifndef LIBQGIT_DB_ODB_H
#define LIBQGIT_DB_ODB_H

#include <libqgit/common.h>
#include <libqgit/db/odb_backend.h>
#include <libqgit/db/oid.h>
#include <libqgit/types.h>
#include <stddef.h>

BEGIN_DECLS

/* Create a new empty object database with no backends. */
QGIT_EXTERN(int) qgit_odb_new(qgit_odb **out);

/* Create an object database with the default loose backend at objects_dir. */
QGIT_EXTERN(int) qgit_odb_open(qgit_odb **out, const char *objects_dir);

/* Add a backend to an existing odb. Higher priority backends are checked first.
 */
QGIT_EXTERN(int)
qgit_odb_add_backend(qgit_odb *odb, qgit_odb_backend *backend, int priority);

/* Free a previously allocated object database. */
QGIT_EXTERN(void) qgit_odb_free(qgit_odb *odb);

/* Read an object from the database. Caller must call qgit_odb_object_free. */
QGIT_EXTERN(int)
qgit_odb_read(qgit_odb_object **out, qgit_odb *odb, const qgit_oid *id);

/* Read only the type and size of an object without loading its data. */
QGIT_EXTERN(int)
qgit_odb_read_header(size_t *len_p, qgit_obj_type *type_p, qgit_odb *odb,
                     const qgit_oid *id);

/* Write an object into the database. Stores the resulting OID in out. */
QGIT_EXTERN(int)
qgit_odb_write(qgit_oid *out, qgit_odb *odb, const void *data, size_t len,
               qgit_obj_type type);

/* Return 1 if the object exists in the database, 0 otherwise. Return -1 on
   error. */
QGIT_EXTERN(int) qgit_odb_exists(qgit_odb *odb, const qgit_oid *id);

/* Compute the OID of a data buffer without writing it to the database. */
QGIT_EXTERN(int)
qgit_odb_hash(qgit_oid *out, const void *data, size_t len, qgit_obj_type type);

/* Free a previously read odb object. */
QGIT_EXTERN(void) qgit_odb_object_free(qgit_odb_object *object);

/* Return the OID of an odb object. */
QGIT_EXTERN(const qgit_oid *) qgit_odb_object_id(const qgit_odb_object *object);

/* Return the raw decompressed data. The pointer is owned by the object. */
QGIT_EXTERN(const void *) qgit_odb_object_data(const qgit_odb_object *object);

/* Return the size in bytes of the raw data. */
QGIT_EXTERN(size_t) qgit_odb_object_size(const qgit_odb_object *object);

/* Return the type of an odb object. */
QGIT_EXTERN(qgit_obj_type) qgit_odb_object_type(const qgit_odb_object *object);

END_DECLS

#endif
