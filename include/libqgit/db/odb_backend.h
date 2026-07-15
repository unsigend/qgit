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

#include <libqgit/oid.h>

QGIT_BEGIN_DECLS

/* ODB backend vtable, one instance per backend implementation. */
struct qgit_odb_backend {
    qgit_odb
        *odb; /* back-pointer to the owning ODB, set by qgit_odb_add_backend */

    /**
     * Read a full object by its OID.
     *
     * On success the implementation allocates *data_p and writes the
     * uncompressed payload into it, sets *len_p to the payload size in
     * bytes, and sets *type_p to the object type. The caller is
     * responsible for freeing *data_p.
     *
     * @param data_p  output pointer to the allocated payload buffer
     * @param len_p   output pointer to the payload length in bytes
     * @param type_p  output pointer to the object type
     * @param backend this backend instance
     * @param oid     OID of the object to read, must not be NULL
     * @return 0 on success, -1 on error and sets errno
     */
    int (*read)(void **data_p, size_t *len_p, qgit_obj_type *type_p,
                struct qgit_odb_backend *backend, const qgit_oid *oid);

    /**
     * Read a full object matching an abbreviated OID prefix.
     *
     * Behaves like read but accepts a prefix of len hex characters.
     * The remaining bytes of short_id beyond the prefix must be zero.
     * Fails if the prefix matches more than one object.
     *
     * @param full_oid output pointer to receive the resolved full OID
     * @param data_p   output pointer to the allocated payload buffer
     * @param len_p    output pointer to the payload length in bytes
     * @param type_p   output pointer to the object type
     * @param backend  this backend instance
     * @param short_id partial OID with the unused suffix zeroed
     * @param len      number of hex characters in the prefix
     * @return 0 on success, -1 on error and sets errno
     */
    int (*read_prefix)(qgit_oid *full_oid, void **data_p, size_t *len_p,
                       qgit_obj_type *type_p, struct qgit_odb_backend *backend,
                       const qgit_oid *short_id, unsigned int len);

    /**
     * Read only the type and size of an object without loading the full
     * payload.
     *
     * @param len_p   output pointer to the payload length in bytes
     * @param type_p  output pointer to the object type
     * @param backend this backend instance
     * @param oid     OID of the object to inspect, must not be NULL
     * @return 0 on success, -1 on error and sets errno
     */
    int (*read_header)(size_t *len_p, qgit_obj_type *type_p,
                       struct qgit_odb_backend *backend, const qgit_oid *oid);

    /**
     * Write a raw object payload into the backend.
     *
     * The implementation computes the OID from the data and type, stores
     * the object, and writes the resulting OID into *oid.
     *
     * @param oid     output pointer to receive the written object OID
     * @param backend this backend instance
     * @param data    uncompressed object payload
     * @param len     payload length in bytes
     * @param type    object type
     * @return 0 on success, -1 on error and sets errno
     */
    int (*write)(qgit_oid *oid, struct qgit_odb_backend *backend,
                 const void *data, size_t len, qgit_obj_type type);

    /**
     * Test whether the backend contains an object with the given OID.
     *
     * @param backend this backend instance
     * @param oid     OID to look up, must not be NULL
     * @return 1 if the object exists, 0 if not, -1 on error and sets errno
     */
    int (*exists)(struct qgit_odb_backend *backend, const qgit_oid *oid);

    /**
     * Release all resources held by this backend.
     *
     * @param backend this backend instance, must not be NULL
     */
    void (*free)(struct qgit_odb_backend *backend);
};

/**
 * Allocate a loose-object backend that reads and writes zlib-compressed
 * objects under the standard two-character fanout layout inside
 * objects_dir.
 *
 * @param backend_out output pointer to receive the new backend, must not be
 * NULL
 * @param objects_dir path to the objects directory (e.g. ".qgit/objects")
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int)
qgit_odb_backend_loose(qgit_odb_backend **backend_out, const char *objects_dir);

QGIT_END_DECLS

#endif
