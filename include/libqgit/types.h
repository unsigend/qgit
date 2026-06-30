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

#ifndef LIBQGIT_TYPES_H
#define LIBQGIT_TYPES_H

typedef struct qgit_oid qgit_oid;
typedef struct qgit_odb qgit_odb;
typedef struct qgit_odb_object qgit_odb_object;
typedef struct qgit_odb_backend qgit_odb_backend;

typedef struct qgit_index qgit_index;
typedef struct qgit_config qgit_config;
typedef struct qgit_reference qgit_reference;
typedef struct qgit_repository qgit_repository;

typedef struct qgit_object qgit_object;
typedef struct qgit_blob qgit_blob;
typedef struct qgit_tree qgit_tree;
typedef struct qgit_tag qgit_tag;
typedef struct qgit_commit qgit_commit;

typedef enum {
    QGIT_OBJ_ANY = -2,   /* Any object type */
    QGIT_OBJ_BAD = -1,   /* Bad object */
    QGIT_OBJ_COMMIT = 1, /* Commit object */
    QGIT_OBJ_BLOB = 2,   /* Blob object */
    QGIT_OBJ_TREE = 3,   /* Tree object */
    QGIT_OBJ_TAG = 4,    /* Tag object */
} qgit_obj_type;

/* Reference type: direct (points to an OID) or symbolic (points to another
   reference name). */
typedef enum {
    QGIT_REF_INVALID = 0,  /* Invalid reference */
    QGIT_REF_OID = 1,      /* Direct reference (points to an OID) */
    QGIT_REF_SYMBOLIC = 2, /* Symbolic reference (points to another ref) */
} qgit_ref_type;

#endif