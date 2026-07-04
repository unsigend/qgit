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

#include <libqgit/db/odb_backend.h>

struct qgit_loose_backend {
    struct qgit_odb_backend backend;
    char *objects_dir;
};

/**
 * Get the path to the object file.
 *
 * @param objects_dir the directory containing the objects
 * @param oid the OID of the object
 * @param path the path to the object file
 * @param pathlen the length of the path
 * @return 0 on success, -1 on error
 */
QGIT_INTERNAL(int)
qgit_loose_backend_path(const char *objects_dir, const qgit_oid *oid,
                        char *path, size_t pathlen);

/**
 * Read an object from the loose backend.
 *
 * @param data_p the data of the object
 * @param len_p the length of the object
 * @param type_p the type of the object
 * @param backend the backend instance
 * @param oid the OID of the object
 * @return 0 on success, -1 on error
 */
QGIT_INTERNAL(int)
qgit_loose_backend_read(void **data_p, size_t *len_p, qgit_obj_type *type_p,
                        struct qgit_odb_backend *backend, const qgit_oid *oid);

/**
 * Read an object from the loose backend with a prefix.
 *
 * @param full_oid the full OID of the object
 * @param data_p the data of the object
 * @param len_p the length of the object
 * @param type_p the type of the object
 * @param backend the backend instance
 * @param short_id the short OID of the object
 * @param len the length of the prefix
 * @return 0 on success, -1 on error
 */
QGIT_INTERNAL(int)
qgit_loose_backend_read_prefix(qgit_oid *full_oid, void **data_p, size_t *len_p,
                               qgit_obj_type *type_p,
                               struct qgit_odb_backend *backend,
                               const qgit_oid *short_id, unsigned int len);

/**
 * Read the header of an object from the loose backend.
 *
 * @param len_p the length of the object
 * @param type_p the type of the object
 * @param backend the backend instance
 * @param oid the OID of the object
 * @return 0 on success, -1 on error
 */
QGIT_INTERNAL(int)
qgit_loose_backend_read_header(size_t *len_p, qgit_obj_type *type_p,
                               struct qgit_odb_backend *backend,
                               const qgit_oid *oid);

/**
 * Write an object to the loose backend.
 *
 * @param oid the OID of the object
 * @param backend the backend instance
 * @param data the data of the object
 * @param len the length of the object
 * @param type the type of the object
 * @return 0 on success, -1 on error
 */
QGIT_INTERNAL(int)
qgit_loose_backend_write(qgit_oid *oid, struct qgit_odb_backend *backend,
                         const void *data, size_t len, qgit_obj_type type);

/**
 * Check if an object exists in the loose backend.
 *
 * @param backend the backend instance
 * @param oid the OID of the object
 * @return 1 if the object exists, 0 if it does not, -1 on error
 */
QGIT_INTERNAL(int)
qgit_loose_backend_exists(struct qgit_odb_backend *backend,
                          const qgit_oid *oid);

/**
 * Free the loose backend.
 *
 * @param backend the backend instance
 */
QGIT_INTERNAL(void)
qgit_loose_backend_free(struct qgit_odb_backend *backend);

#include "../../rawobj/rawobj.h"

#endif