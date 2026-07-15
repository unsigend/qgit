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

#ifndef LIBQGIT_OBJECT_OBJECT_H
#define LIBQGIT_OBJECT_OBJECT_H

#include <libqgit/oid.h>

QGIT_BEGIN_DECLS

/**
 * Look up an object in the repository by OID.
 *
 * Pass QGIT_OBJ_ANY as type to let the function determine the type
 * automatically, otherwise the stored type must match or the call fails.
 * The returned object must be released with qgit_object_free.
 *
 * @param out  output pointer to receive the object handle, must not be NULL
 * @param repo repository to search, must not be NULL
 * @param id   OID of the object to look up, must not be NULL
 * @param type expected object type, or QGIT_OBJ_ANY
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int)
qgit_object_lookup(qgit_object **out, qgit_repository *repo, const qgit_oid *id,
                   qgit_obj_type type);

/**
 * Look up an object in the repository by abbreviated OID prefix.
 *
 * len must be at least QGIT_OID_MINPREFIXLEN and must identify a unique
 * object. Pass QGIT_OBJ_ANY to skip type checking.
 * The returned object must be released with qgit_object_free.
 *
 * @param out  output pointer to receive the object handle, must not be NULL
 * @param repo repository to search, must not be NULL
 * @param id   partial OID with the unused suffix zeroed
 * @param len  number of hex characters in the prefix
 * @param type expected object type, or QGIT_OBJ_ANY
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int)
qgit_object_lookup_prefix(qgit_object **out, qgit_repository *repo,
                          const qgit_oid *id, unsigned int len,
                          qgit_obj_type type);

/**
 * Return the OID of an object.
 *
 * The returned pointer is owned by the object and is valid until
 * qgit_object_free is called.
 *
 * @param obj object to query, must not be NULL
 * @return pointer to the object OID
 */
QGIT_EXTERN(const qgit_oid *) qgit_object_id(const qgit_object *obj);

/**
 * Return the type of an object.
 *
 * @param obj object to query, must not be NULL
 * @return the object type
 */
QGIT_EXTERN(qgit_obj_type) qgit_object_type(const qgit_object *obj);

/**
 * Return the repository that owns this object.
 *
 * The returned pointer is valid as long as the repository is open.
 * Freeing the repository invalidates the object.
 *
 * @param obj object to query, must not be NULL
 * @return pointer to the owning repository
 */
QGIT_EXTERN(qgit_repository *) qgit_object_owner(const qgit_object *obj);

/**
 * Free an object handle.
 *
 * @param obj object to free, no-op if NULL
 */
QGIT_EXTERN(void) qgit_object_free(qgit_object *obj);

/**
 * Create an in-memory deep copy of an object. The copy must be explicitly
 * freed.
 *
 * @param dest   output pointer to receive the copy, must not be NULL
 * @param source object to copy, must not be NULL
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int) qgit_object_dup(qgit_object **dest, const qgit_object *source);

/**
 * Recursively peel an object until an object of the specified type is met. The
 * returned object must be freed with qgit_object_free.
 *
 * If target_type is QGIT_OBJ_ANY, the object is peeled until the type
 * changes (e.g. a tag is chased until the referenced object is no longer
 * a tag).
 *
 * @param peeled      output pointer to receive the peeled object, must not be
 *                    NULL
 * @param object      object to peel, must not be NULL
 * @param target_type requested type: QGIT_OBJ_COMMIT, QGIT_OBJ_TREE,
 *                    QGIT_OBJ_BLOB, QGIT_OBJ_TAG, or QGIT_OBJ_ANY
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int)
qgit_object_peel(qgit_object **peeled, const qgit_object *object,
                 qgit_obj_type target_type);

/**
 * Convert a qgit_obj_type to its canonical string representation.
 *
 * The returned pointer refers to static storage and must not be freed.
 * Returns NULL for QGIT_OBJ_BAD or unknown types.
 *
 * @param type object type to convert
 * @return "commit", "tree", "blob", "tag", or NULL
 */
QGIT_EXTERN(const char *) qgit_object_type2string(qgit_obj_type type);

/**
 * Parse a type string into a qgit_obj_type.
 *
 * @param str type string to parse (e.g. "commit", "tree", "blob", "tag")
 * @return the corresponding qgit_obj_type, or QGIT_OBJ_BAD if unrecognized
 */
QGIT_EXTERN(qgit_obj_type) qgit_object_string2type(const char *str);

/**
 * Test whether a type is a valid loose object type.
 *
 * @param type type to test
 * @return 1 if type is QGIT_OBJ_COMMIT, QGIT_OBJ_TREE, QGIT_OBJ_BLOB,
 *         or QGIT_OBJ_TAG, 0 otherwise
 */
QGIT_EXTERN(int) qgit_object_typeisloose(qgit_obj_type type);

QGIT_END_DECLS

#endif
