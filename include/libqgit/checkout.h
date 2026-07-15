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

#ifndef LIBQGIT_CHECKOUT_H
#define LIBQGIT_CHECKOUT_H

#include <libqgit/common.h>
#include <libqgit/types.h>

QGIT_BEGIN_DECLS

/**
 * Check out a tree into a new directory on disk.
 *
 * Creates path and recursively writes each tree entry under it. Blob
 * entries become files. Tree entries become subdirectories. path must
 * not already exist. This call does not update HEAD or the index.
 *
 * @param repo repository that owns the tree, must not be NULL
 * @param tree tree to materialize, must not be NULL
 * @param path destination directory path, must not be NULL
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int)
qgit_checkout_tree(qgit_repository *repo, const qgit_tree *tree,
                   const char *path);

QGIT_END_DECLS

#endif
