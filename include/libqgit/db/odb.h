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

/**
 * Create a new object database with no backends.
 *
 * Before the ODB can be used for read/writing, a custom database
 * backend must be manually added using `qgit_odb_add_backend()`
 *
 * @param out location to store the database pointer, if opened.
 *			Set to NULL if the open failed
 * @return 0 on success, -1 on error and set errno
 */
QGIT_EXTERN(int) qgit_odb_new(qgit_odb **out);

/**
 * Create a new object database and automatically add
 * the default loose backend.
 *
 * @param out location to store the database pointer, if opened.
 *			Set to NULL if the open failed
 * @param objects_dir path of the backend's "objects" directory
 * @return 0 on success, -1 on error and set errno
 */
QGIT_EXTERN(int) qgit_odb_open(qgit_odb **out, const char *objects_dir);

/**
 * Add a custom backend to an existing Object DB.
 *
 * The backends are checked in relative ordering, based on the
 * value of the `priority` parameter. Higher priority backends
 * are checked first.
 *
 * @param odb database to add the backend to
 * @param backend pointer to a qgit_odb_backend instance
 * @param priority value for ordering the backends queue
 * @return 0 on success, -1 on error and set errno
 */
QGIT_EXTERN(int)
qgit_odb_add_backend(qgit_odb *odb, qgit_odb_backend *backend, int priority);

/**
 * Close an open object database.
 *
 * @param odb database pointer to close. If NULL no action is taken
 */
QGIT_EXTERN(void) qgit_odb_free(qgit_odb *odb);

/**
 * Read an object from the database.
 *
 * This method queries all available ODB backends
 * trying to read the given OID.
 *
 * The returned object must be closed by the user once
 * it is no longer in use with `qgit_odb_object_free()`.
 *
 * @param out pointer where to store the read object
 * @param odb database to search for the object in
 * @param id identity of the object to read
 * @return 0 on success, -1 on error and set errno
 */
QGIT_EXTERN(int)
qgit_odb_read(qgit_odb_object **out, qgit_odb *odb, const qgit_oid *id);

/**
 * Read an object from the database, given a prefix
 * of its identifier.
 *
 * This method queries all available ODB backends
 * trying to match the `len` first hexadecimal
 * characters of the `short_id`.
 * The prefix must be long enough to identify
 * a unique object in all the backends, the
 * method will fail otherwise.
 *
 * The returned object must be closed by the user once
 * it is no longer in use with `qgit_odb_object_free()`.
 *
 * @param out pointer where to store the read object
 * @param odb database to search for the object in
 * @param short_id a prefix of the id of the object to read
 * @param len the length of the prefix
 * @return 0 on success, -1 on error and set errno
 */
QGIT_EXTERN(int)
qgit_odb_read_prefix(qgit_odb_object **out, qgit_odb *odb,
                     const qgit_oid *short_id, unsigned int len);

/**
 * Read the header of an object from the database, without
 * reading its full contents.
 *
 * The header includes the length and the type of an object.
 *
 * @param len_p pointer where to store the length
 * @param type_p pointer where to store the type
 * @param odb database to search for the object in
 * @param id identity of the object to read
 * @return 0 on success, -1 on error and set errno
 */
QGIT_EXTERN(int)
qgit_odb_read_header(size_t *len_p, qgit_obj_type *type_p, qgit_odb *odb,
                     const qgit_oid *id);

/**
 * Write an object directly into the ODB.
 *
 * @param out pointer to store the OID result of the write
 * @param odb object database where to store the object
 * @param data buffer with the data to store
 * @param len size of the buffer
 * @param type type of the data to store
 * @return 0 on success, -1 on error and set errno
 */
QGIT_EXTERN(int)
qgit_odb_write(qgit_oid *out, qgit_odb *odb, const void *data, size_t len,
               qgit_obj_type type);

/**
 * Determine if the given object can be found in the object database.
 *
 * @param odb database to be searched for the given object
 * @param id the object to search for
 * @return 1 if the object was found, 0 if not, -1 on error and set errno
 */
QGIT_EXTERN(int) qgit_odb_exists(qgit_odb *odb, const qgit_oid *id);

/**
 * Determine the object ID of a data buffer.
 *
 * The resulting OID will be the identifier for the data
 * buffer as if the data buffer were to be written to the ODB.
 *
 * @param out the resulting object ID
 * @param data data to hash
 * @param len size of the data
 * @param type type of the data to hash
 * @return 0 on success, -1 on error and set errno
 */
QGIT_EXTERN(int)
qgit_odb_hash(qgit_oid *out, const void *data, size_t len, qgit_obj_type type);

/**
 * Close an ODB object.
 *
 * This method must always be called once a `qgit_odb_object` is no
 * longer needed, otherwise memory will leak.
 *
 * @param object object to close
 */
QGIT_EXTERN(void) qgit_odb_object_free(qgit_odb_object *object);

/**
 * Return the OID of an ODB object.
 *
 * This is the OID from which the object was read.
 *
 * @param object the object
 * @return a pointer to the OID
 */
QGIT_EXTERN(const qgit_oid *) qgit_odb_object_id(const qgit_odb_object *object);

/**
 * Return the data of an ODB object.
 *
 * This is the uncompressed, raw data as read from the ODB,
 * without the leading header.
 *
 * This pointer is owned by the object and shall not be free'd.
 *
 * @param object the object
 * @return a pointer to the data
 */
QGIT_EXTERN(const void *) qgit_odb_object_data(const qgit_odb_object *object);

/**
 * Return the size of an ODB object.
 *
 * This is the real size of the `data` buffer, not the
 * actual size of the object.
 *
 * @param object the object
 * @return the size
 */
QGIT_EXTERN(size_t) qgit_odb_object_size(const qgit_odb_object *object);

/**
 * Return the type of an ODB object.
 *
 * @param object the object
 * @return the type
 */
QGIT_EXTERN(qgit_obj_type) qgit_odb_object_type(const qgit_odb_object *object);

END_DECLS

#endif
