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

#ifndef TREE_H
#define TREE_H

#include "../../odb/rawobj/rawobj.h"
#include "../object/object.h"
#include "libqgit/common.h"

struct qgit_tree_entry {
    unsigned int
        mode;   /* UNIX file mode for the entry (e.g. 0100644, 0040000) */
    char *path; /* path of the entry */
    qgit_oid oid;
};

/* Raw payload format for tree:
     <mode> <path>\0<sha1-20>
     ...
     <mode> <path>\0<sha1-20>
*/
struct qgit_tree {
    qgit_object object;
    struct vector *entries;
};

struct qgit_treebuilder {
    struct vector *entries;
};

/**
 * Parse a tree object from an ODB object.
 *
 * @param out output pointer to receive the tree object, must not be NULL
 * @param odb_obj ODB object to parse from, must not be NULL
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_INTERNAL(int) tree_parse(qgit_tree *out, qgit_odb_object *odb_obj);

/**
 * Free a tree object.
 *
 * @param tree tree object to free
 */
QGIT_INTERNAL(void) tree_free(qgit_tree *tree);

/**
 * Free a tree entry.
 *
 * @param p tree entry to free
 */
QGIT_INTERNAL(void) tree_entry_free(void *p);

/**
 * Sort a vector of tree entries in Git tree order.
 *
 * Names are compared byte-wise, directory entries sort as if their name
 * had a trailing '/'.
 *
 * @param entries vector of tree entries to sort
 */
QGIT_INTERNAL(void) tree_entry_sort(struct vector *entries);

#endif