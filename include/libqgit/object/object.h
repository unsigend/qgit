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

#include <libqgit/common.h>
#include <libqgit/db/oid.h>
#include <libqgit/types.h>

BEGIN_DECLS

/**
 * Lookup a reference to one of the objects in a repository.
 *
 * The generated reference should be closed with the `qgit_object_free`
 * method instead of free'd manually.
 *
 * The `type` parameter must match the type of the object
 * in the odb, the method will fail otherwise.
 * The special value `QGIT_OBJ_ANY` may be passed to let
 * the method guess the object's type.
 *
 * @param object pointer to the looked-up object
 * @param repo the repository to look up the object
 * @param id the unique identifier for the object
 * @param type the type of the object
 * @return 0 on success, -1 on error and set errno
 */
QGIT_EXTERN(int)
qgit_object_lookup(qgit_object **object, qgit_repository *repo,
                   const qgit_oid *id, qgit_obj_type type);

/**
 * Lookup a reference to one of the objects in a repository,
 * given a prefix of its identifier (short id).
 *
 * The object obtained will be so that its identifier
 * matches the first `len` hexadecimal characters
 * (packets of 4 bits) of the given `id`.
 * `len` must be at least QGIT_OID_MINPREFIXLEN, and
 * long enough to identify a unique object matching
 * the prefix, otherwise the method will fail.
 *
 * The generated reference should be closed with the `qgit_object_free`
 * method instead of free'd manually.
 *
 * The `type` parameter must match the type of the object
 * in the odb, the method will fail otherwise.
 * The special value `QGIT_OBJ_ANY` may be passed to let
 * the method guess the object's type.
 *
 * @param object pointer where to store the looked-up object
 * @param repo the repository to look up the object
 * @param id a short identifier for the object
 * @param len the length of the short identifier
 * @param type the type of the object
 * @return 0 on success, -1 on error and set errno
 */
QGIT_EXTERN(int)
qgit_object_lookup_prefix(qgit_object **object, qgit_repository *repo,
                          const qgit_oid *id, unsigned int len,
                          qgit_obj_type type);

/**
 * Get the id of a repository object.
 *
 * The pointer is valid for the object's lifetime.
 *
 * @param obj the repository object
 * @return a pointer to the object id
 */
QGIT_EXTERN(const qgit_oid *) qgit_object_id(const qgit_object *obj);

/**
 * Get the object type of an object.
 *
 * @param obj the repository object
 * @return the object's type
 */
QGIT_EXTERN(qgit_obj_type) qgit_object_type(const qgit_object *obj);

/**
 * Get the repository that owns this object.
 *
 * @param obj the object
 * @return the repository that owns this object
 */
QGIT_EXTERN(qgit_repository *) qgit_object_owner(const qgit_object *obj);

/**
 * Close an open object.
 *
 * This method instructs the library to close an existing
 * object. It must be called when the object is no longer needed,
 * otherwise memory will leak.
 *
 * @param object the object to close
 */
QGIT_EXTERN(void) qgit_object_free(qgit_object *object);

/**
 * Convert an object type to its string representation.
 *
 * The result is a pointer to a string in static memory and
 * should not be free'd.
 *
 * @param type object type to convert
 * @return the corresponding string representation
 */
QGIT_EXTERN(const char *) qgit_object_type2string(qgit_obj_type type);

/**
 * Convert a string object type representation to a qgit_obj_type.
 *
 * @param str the string to convert
 * @return the corresponding qgit_obj_type
 */
QGIT_EXTERN(qgit_obj_type) qgit_object_string2type(const char *str);

/**
 * Determine if the given qgit_obj_type is a valid loose object type.
 *
 * @param type object type to test
 * @return 1 if the type represents a valid loose object type,
 * 0 otherwise
 */
QGIT_EXTERN(int) qgit_object_typeisloose(qgit_obj_type type);

END_DECLS

#endif
