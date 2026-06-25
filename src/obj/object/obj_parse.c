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

#include "error.h"
#include "obj/object.h"

int obj_parse(struct object *obj)
{
  if (!obj)
    return -1;
  if (obj->parsed)
    return 0;

  switch (obj->type) {
  case OBJ_COMMIT:
    if (commit_parse(obj) == -1)
      return -1;
    break;
  case OBJ_BLOB:
    if (blob_parse(obj) == -1)
      return -1;
    break;
  case OBJ_TREE:
    if (tree_parse(obj) == -1)
      return -1;
    break;
  case OBJ_TAG:
    if (tag_parse(obj) == -1)
      return -1;
    break;
  default: {
    setqerrno(QE_INVALIDOBJ);
    return -1;
  }
  }
  obj->parsed = 1;
  return 0;
}