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
#include <stdio.h>

#include "obj/obj.h"

int obj_fprintf(FILE *stream, struct obj *obj)
{
  if (!stream || !obj) {
    errno = EINVAL;
    return -1;
  }

  switch (obj->type) {
  case OBJ_BLOB:
    return blob_fprintf(stream, obj);
  case OBJ_COMMIT:
    return commit_fprintf(stream, obj);
  case OBJ_TREE:
    return tree_fprintf(stream, obj);
  case OBJ_TAG:
    return tag_fprintf(stream, obj);
  default:
    errno = EINVAL;
    return -1;
  }
}

int obj_printf(struct obj *obj) { return obj_fprintf(stdout, obj); }