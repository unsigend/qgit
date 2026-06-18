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

#include <stdlib.h>
#include <string.h>

#include "obj/object.h"

static void delegate(struct obj *obj)
{
  switch (obj->type) {
  case OBJ_COMMIT:
    commit_close(&obj->commit);
    break;
  case OBJ_BLOB:
    blob_close(&obj->blob);
    break;
  case OBJ_TREE:
    tree_close(&obj->tree);
    break;
  case OBJ_TAG:
    tag_close(&obj->tag);
    break;
  default:
    break;
  }
}

void obj_close(struct obj *obj)
{
  if (!obj)
    return;
  if (obj->payload)
    free(obj->payload);
  delegate(obj);
  memset(obj, 0, sizeof(struct obj));
  free(obj);
}