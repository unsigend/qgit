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

#include "obj/blob.h"
#include "obj/commit.h"
#include "obj/obj.h"
#include "obj/tag.h"
#include "obj/tree.h"

int obj_parse(struct obj *obj)
{
  if (!obj) {
    errno = EINVAL;
    return -1;
  }

  switch (obj->type) {
  case OBJ_BLOB:
    return blob_parse(obj);
  case OBJ_COMMIT:
    return commit_parse(obj);
  case OBJ_TREE:
    return tree_parse(obj);
  case OBJ_TAG:
    return tag_parse(obj);
  default:
    break;
  }
  return -1;
}