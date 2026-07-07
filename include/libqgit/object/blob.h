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

#ifndef LIBQGIT_OBJECT_BLOB_H
#define LIBQGIT_OBJECT_BLOB_H

#include <libqgit/common.h>
#include <libqgit/object/object.h>
#include <libqgit/oid.h>
#include <libqgit/types.h>
#include <stddef.h>

QGIT_BEGIN_DECLS

/**
 * Look up a blob object in a repository by OID.
 *
 * @param out  output pointer to receive the blob handle, must not be NULL
 * @param repo repository to search, must not be NULL
 * @param id   OID of the blob, must not be NULL
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_INLINE(int)
qgit_blob_lookup(qgit_blob **out, qgit_repository *repo, const qgit_oid *id)
{
    return qgit_object_lookup((qgit_object **)out, repo, id, QGIT_OBJ_BLOB);
}

/**
 * Look up a blob object in a repository by abbreviated OID prefix.
 *
 * @param out  output pointer to receive the blob handle, must not be NULL
 * @param repo repository to search, must not be NULL
 * @param id   partial OID with the unused suffix zeroed
 * @param len  number of hex characters in the prefix
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_INLINE(int)
qgit_blob_lookup_prefix(qgit_blob **out, qgit_repository *repo,
                        const qgit_oid *id, unsigned int len)
{
    return qgit_object_lookup_prefix((qgit_object **)out, repo, id, len,
                                     QGIT_OBJ_BLOB);
}

/**
 * Free a blob handle.
 *
 * @param blob blob to free, no-op if NULL
 */
QGIT_INLINE(void) qgit_blob_free(qgit_blob *blob)
{
    qgit_object_free((qgit_object *)blob);
}

/**
 * Return a read-only pointer to the raw content of a blob.
 *
 * The pointer is owned by the blob and is valid until qgit_blob_free is
 * called.
 *
 * @param blob blob to query, must not be NULL
 * @return pointer to the raw payload, or NULL if the blob is empty
 */
QGIT_EXTERN(const void *) qgit_blob_rawcontent(qgit_blob *blob);

/**
 * Return the size in bytes of the raw content of a blob.
 *
 * @param blob blob to query, must not be NULL
 * @return payload size in bytes
 */
QGIT_EXTERN(size_t) qgit_blob_rawsize(qgit_blob *blob);

/**
 * Read a file relative to the repository working directory and write it
 * to the ODB as a loose blob.
 *
 * @param oid  output pointer to receive the written blob OID, must not be NULL
 * @param repo repository to write into, must not be NULL
 * @param path file path relative to the repository working directory
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int)
qgit_blob_create_fromfile(qgit_oid *oid, qgit_repository *repo,
                          const char *path);

/**
 * Read a file by absolute path and write it to the ODB as a loose blob.
 *
 * @param oid  output pointer to receive the written blob OID, must not be NULL
 * @param repo repository to write into, must not be NULL
 * @param path absolute path to the file to read
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int)
qgit_blob_create_fromdisk(qgit_oid *oid, qgit_repository *repo,
                          const char *path);

/**
 * Write an in-memory buffer to the ODB as a loose blob.
 *
 * @param oid    output pointer to receive the written blob OID, must not be
 * NULL
 * @param repo   repository to write into, must not be NULL
 * @param buffer data to store as the blob payload
 * @param len    length of the buffer in bytes
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int)
qgit_blob_create_frombuffer(qgit_oid *oid, qgit_repository *repo,
                            const void *buffer, size_t len);

QGIT_END_DECLS

#endif
