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

#include <string.h>

#include "error.h"
#include "obj/object.h"

enum obj_type obj_type_from_str(const char *str)
{
  if (!str)
    return OBJ_NONE;
  if (strcmp(str, "commit") == 0)
    return OBJ_COMMIT;
  if (strcmp(str, "blob") == 0)
    return OBJ_BLOB;
  if (strcmp(str, "tree") == 0)
    return OBJ_TREE;
  if (strcmp(str, "tag") == 0)
    return OBJ_TAG;
  setqerrno(QE_INVALIDOBJ);
  return OBJ_NONE;
}

const char *obj_type_to_str(enum obj_type type)
{
  switch (type) {
  case OBJ_COMMIT:
    return "commit";
  case OBJ_BLOB:
    return "blob";
  case OBJ_TREE:
    return "tree";
  case OBJ_TAG:
    return "tag";
  default:
    setqerrno(QE_INVALIDOBJ);
    return NULL;
  }
}