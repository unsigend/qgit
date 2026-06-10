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

#include <errno.h>
#include <stdlib.h>

#include "obj/blob.h"
#include "obj/commit.h"
#include "obj/obj.h"
#include "obj/tag.h"
#include "obj/tree.h"

void obj_close(struct obj *obj)
{
  if (!obj)
    return;

  switch (obj->type) {
  case OBJ_BLOB:
    blob_free(&obj->blob);
    break;
  case OBJ_COMMIT:
    commit_free(&obj->commit);
    break;
  case OBJ_TREE:
    tree_free(&obj->tree);
    break;
  case OBJ_TAG:
    tag_free(&obj->tag);
    break;
  default:
    break;
  }
  if (obj->payload)
    free(obj->payload);
  free(obj);
}