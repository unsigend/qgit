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

typedef struct qgit_oid qgit_oid; /* SHA-1 object identifier */
typedef struct qgit_odb qgit_odb; /* opaque object database handle */
typedef struct qgit_odb_backend
    qgit_odb_backend; /* opaque ODB backend vtable */
typedef struct qgit_odb_object
    qgit_odb_object; /* opaque object read from the ODB */
typedef struct qgit_object
    qgit_object;                                  /* opaque git object handle (commit, tree, blob, tag) */
typedef struct qgit_blob qgit_blob;               /* opaque blob object */
typedef struct qgit_commit qgit_commit;           /* opaque commit object */
typedef struct qgit_tag qgit_tag;                 /* opaque annotated tag object */
typedef struct qgit_tree qgit_tree;               /* opaque tree object */
typedef struct qgit_tree_entry qgit_tree_entry;   /* opaque tree entry */
typedef struct qgit_treebuilder qgit_treebuilder;  /* opaque in-memory tree builder */
typedef struct qgit_time qgit_time;               /* timestamp with timezone offset */
typedef struct qgit_signature qgit_signature;     /* name, email, and time of an action */
typedef struct qgit_repository qgit_repository;   /* opaque repository handle */
typedef struct qgit_reference qgit_reference;   /* opaque reference handle */
typedef struct qgit_config qgit_config;         /* opaque config handle */
typedef struct qgit_index qgit_index;             /* opaque index handle */
typedef struct qgit_revwalk qgit_revwalk;         /* opaque commit graph walker */

/* Branch type, used to filter branch listings and operations. */
typedef enum {
    QGIT_BRANCH_LOCAL = 1,  /* local branch under refs/heads/ */
    QGIT_BRANCH_REMOTE = 2, /* remote branch under refs/remotes/ */
    QGIT_BRANCH_ALL =
        QGIT_BRANCH_LOCAL | QGIT_BRANCH_REMOTE, /* local and remote branches */
} qgit_branch_type;

/* Reference type. */
typedef enum {
    QGIT_REF_INVALID = 0,  /* invalid reference */
    QGIT_REF_DIRECT = 1,   /* direct reference */
    QGIT_REF_SYMBOLIC = 2, /* symbolic reference */
    QGIT_REF_REMOTE = 4,   /* remote reference */
} qgit_ref_type;

/* Object type identifiers. */
typedef enum {
    QGIT_OBJ_ANY = -2,   /* any object type */
    QGIT_OBJ_BAD = -1,   /* invalid  object type */
    QGIT_OBJ_COMMIT = 1, /* commit object */
    QGIT_OBJ_TREE = 2,   /* tree object */
    QGIT_OBJ_BLOB = 3,   /* blob object */
    QGIT_OBJ_TAG = 4,    /* tag object */
} qgit_obj_type;

#endif