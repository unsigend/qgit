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

#include "../blob/blob.h"
#include "../commit/commit.h"
#include "../tag/tag.h"
#include "../tree/tree.h"

#include <assert.h>
#include <libqgit/error.h>

size_t qgit_object_mem_size(qgit_obj_type type)
{
    switch (type) {
    case QGIT_OBJ_COMMIT:
        return sizeof(qgit_commit);
    case QGIT_OBJ_TAG:
        return sizeof(qgit_tag);
    case QGIT_OBJ_TREE:
        return sizeof(qgit_tree);
    case QGIT_OBJ_BLOB:
        return sizeof(qgit_blob);
    default:
        qgit_seterror(QGITERR_BADREFTYPE);
        return 0;
    }
}