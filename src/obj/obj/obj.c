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
#include <string.h>

#include "obj/obj.h"

obj_type_t obj_type_from_str(const char *str)
{
  if (!str) {
    errno = EINVAL;
    return OBJ_NONE;
  }

  if (strcmp(str, "blob") == 0)
    return OBJ_BLOB;
  else if (strcmp(str, "commit") == 0)
    return OBJ_COMMIT;
  else if (strcmp(str, "tree") == 0)
    return OBJ_TREE;
  else if (strcmp(str, "tag") == 0)
    return OBJ_TAG;
  else {
    errno = EINVAL;
    return OBJ_NONE;
  }
}

const char *str_from_obj_type(obj_type_t type)
{
  if (type == OBJ_BLOB)
    return "blob";
  else if (type == OBJ_COMMIT)
    return "commit";
  else if (type == OBJ_TREE)
    return "tree";
  else if (type == OBJ_TAG)
    return "tag";
  else {
    errno = EINVAL;
    return NULL;
  }
}
