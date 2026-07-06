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
#include "object.h"

#include <stdlib.h>

void qgit_object_free(qgit_object *obj)
{
    if (!obj)
        return;

    /* delegate to the subtype, the object layer free only responsible for the
      base pointer */
    switch (obj->type) {
    case QGIT_OBJ_COMMIT:
        commit_free((qgit_commit *)obj);
        break;
    case QGIT_OBJ_TAG:
        tag_free((qgit_tag *)obj);
        break;
    case QGIT_OBJ_TREE:
        tree_free((qgit_tree *)obj);
        break;
    case QGIT_OBJ_BLOB:
        blob_free((qgit_blob *)obj);
        break;
    default:
        break;
    }

    free(obj);
}
