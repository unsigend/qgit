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
#include <libqgit/oid.h>
#include <libqgit/types.h>
#include <stddef.h>

QGIT_BEGIN_DECLS

/**
 * Allocate an empty object database with no backends attached.
 *
 * At least one backend must be added via qgit_odb_add_backend before
 * the ODB can be used for reading or writing.
 *
 * @param out output pointer to receive the new ODB handle, must not be NULL
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int) qgit_odb_new(qgit_odb **out);

/**
 * Open an object database backed by a loose-object store.
 *
 * Equivalent to qgit_odb_new followed by qgit_odb_add_backend with a
 * loose backend rooted at objects_dir.
 *
 * @param out         output pointer to receive the ODB handle, must not be NULL
 * @param objects_dir path to the objects directory (e.g. ".qgit/objects")
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int) qgit_odb_open(qgit_odb **out, const char *objects_dir);

/**
 * Attach a custom backend to an existing ODB.
 *
 * Backends are queried in ascending priority order; higher priority values
 * are checked first. The ODB takes ownership of the backend and will call
 * backend->free when the ODB is freed.
 *
 * @param odb      ODB to attach the backend to, must not be NULL
 * @param backend  backend to attach, must not be NULL
 * @param priority ordering value, higher means checked first
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int)
qgit_odb_add_backend(qgit_odb *odb, qgit_odb_backend *backend, int priority);

/**
 * Free an ODB handle and all attached backends.
 *
 * @param odb ODB to free, no-op if NULL
 */
QGIT_EXTERN(void) qgit_odb_free(qgit_odb *odb);

/**
 * Read a full object from the ODB.
 *
 * Queries each backend in priority order until the object is found.
 * The returned qgit_odb_object must be released with qgit_odb_object_free.
 *
 * @param out output pointer to receive the object handle, must not be NULL
 * @param odb ODB to search, must not be NULL
 * @param oid OID of the object to read, must not be NULL
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int)
qgit_odb_read(qgit_odb_object **out, qgit_odb *odb, const qgit_oid *oid);

/**
 * Read a full object from the ODB using an abbreviated OID prefix.
 *
 * len must be at least QGIT_OID_MINPREFIXLEN. Fails if the prefix
 * matches more than one object.
 *
 * @param out      output pointer to receive the object handle, must not be NULL
 * @param odb      ODB to search, must not be NULL
 * @param short_id partial OID with the unused suffix zeroed
 * @param len      number of hex characters in the prefix
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int)
qgit_odb_read_prefix(qgit_odb_object **out, qgit_odb *odb,
                     const qgit_oid *short_id, unsigned int len);

/**
 * Read only the type and uncompressed size of an object without loading
 * the full payload.
 *
 * @param len_p  output pointer to receive the payload size in bytes
 * @param type_p output pointer to receive the object type
 * @param odb    ODB to search, must not be NULL
 * @param oid    OID of the object to inspect, must not be NULL
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int)
qgit_odb_read_header(size_t *len_p, qgit_obj_type *type_p, qgit_odb *odb,
                     const qgit_oid *oid);

/**
 * Test whether an object with the given OID exists in the ODB.
 *
 * @param odb ODB to search, must not be NULL
 * @param oid OID to look up, must not be NULL
 * @return 1 if found, 0 if not found, -1 on error and sets errno
 */
QGIT_EXTERN(int) qgit_odb_exists(qgit_odb *odb, const qgit_oid *oid);

/**
 * Write a raw object payload into the ODB.
 *
 * Computes the SHA-1 OID from the header and data, then delegates to the
 * first writable backend. The resulting OID is written into *oid.
 *
 * @param oid  output pointer to receive the written object OID, must not be
 * NULL
 * @param odb  ODB to write into, must not be NULL
 * @param data uncompressed object payload
 * @param len  payload length in bytes
 * @param type object type
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int)
qgit_odb_write(qgit_oid *oid, qgit_odb *odb, const void *data, size_t len,
               qgit_obj_type type);

/**
 * Compute the OID that a data buffer would receive if written to the ODB.
 *
 * No object is stored, only the hash is computed. Used by hash-object.
 *
 * @param oid  output pointer to receive the computed OID, must not be NULL
 * @param data buffer to hash
 * @param len  buffer length in bytes
 * @param type object type to use when constructing the git header
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int)
qgit_odb_hash(qgit_oid *oid, const void *data, size_t len, qgit_obj_type type);

/**
 * Compute the OID that a file would receive if written to the ODB as the
 * given object type.
 *
 * No object is stored. Equivalent to git hash-object without -w.
 *
 * @param oid  output pointer to receive the computed OID, must not be NULL
 * @param path path to the file to hash
 * @param type object type to use when constructing the git header
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int)
qgit_odb_hashfile(qgit_oid *oid, const char *path, qgit_obj_type type);

/**
 * Free a qgit_odb_object returned by qgit_odb_read or qgit_odb_read_prefix.
 *
 * @param object object to free, no-op if NULL
 */
QGIT_EXTERN(void) qgit_odb_object_free(qgit_odb_object *object);

/**
 * Return the OID of an ODB object.
 *
 * The returned pointer is owned by the object and is valid until
 * qgit_odb_object_free is called.
 *
 * @param object object to query, must not be NULL
 * @return pointer to the object OID
 */
QGIT_EXTERN(const qgit_oid *) qgit_odb_object_id(qgit_odb_object *object);

/**
 * Return the uncompressed payload of an ODB object.
 *
 * The returned pointer is owned by the object and is valid until
 * qgit_odb_object_free is called. The data does not include the git
 * object header.
 *
 * @param object object to query, must not be NULL
 * @return pointer to the raw payload
 */
QGIT_EXTERN(const void *) qgit_odb_object_data(qgit_odb_object *object);

/**
 * Return the uncompressed payload size of an ODB object.
 *
 * @param object object to query, must not be NULL
 * @return payload size in bytes
 */
QGIT_EXTERN(size_t) qgit_odb_object_size(qgit_odb_object *object);

/**
 * Return the type of an ODB object.
 *
 * @param object object to query, must not be NULL
 * @return object type
 */
QGIT_EXTERN(qgit_obj_type) qgit_odb_object_type(qgit_odb_object *object);

QGIT_END_DECLS

#endif
