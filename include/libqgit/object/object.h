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

/* Lookup an object in the repository by OID. Pass QGIT_OBJ_ANY to infer the
   type automatically. */
QGIT_EXTERN(int)
qgit_object_lookup(qgit_object **object, qgit_repository *repo,
                   const qgit_oid *id, qgit_obj_type type);

/* Lookup an object by a short OID prefix (at least QGIT_OID_MINPREFIXLEN hex
   chars). The prefix must uniquely identify one object, fails if ambiguous. */
QGIT_EXTERN(int)
qgit_object_lookup_prefix(qgit_object **object, qgit_repository *repo,
                          const qgit_oid *id, unsigned int len,
                          qgit_obj_type type);

/* Return the OID of the object. The pointer is valid for the object's lifetime.
 */
QGIT_EXTERN(const qgit_oid *) qgit_object_id(const qgit_object *obj);

/* Return the type of the object. */
QGIT_EXTERN(qgit_obj_type) qgit_object_type(const qgit_object *obj);

/* Return the repository that owns this object. */
QGIT_EXTERN(qgit_repository *) qgit_object_owner(const qgit_object *obj);

/* Release an object obtained from a lookup. */
QGIT_EXTERN(void) qgit_object_free(qgit_object *object);

/* Convert a qgit_obj_type to its string representation. */
QGIT_EXTERN(const char *) qgit_object_type2string(qgit_obj_type type);

/* Convert a string object type representation to a qgit_obj_type. */
QGIT_EXTERN(qgit_obj_type) qgit_object_string2type(const char *str);

/* Return 1 if the type is a valid loose object type, 0 otherwise. */
QGIT_EXTERN(int) qgit_object_typeisloose(qgit_obj_type type);

END_DECLS

#endif