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

struct qgit_tree { /* TODO*/
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

#endif