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

#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "obj/obj_internal.h"
#include "obj/object.h"

int obj_write(struct object *obj, struct repo *repo)
{
  if (!obj || !repo)
    return -1;

  void *buf = NULL;
  size_t buflen = 0;

  if (obj_write_buf(obj, &buf, &buflen) == -1)
    return -1;

  if (sha1(buf, buflen, obj->sha1) == -1) {
    free(buf);
    return -1;
  }

  if (obj_store(repo, obj->sha1, buf, buflen) == -1) {
    free(buf);
    return -1;
  }

  free(buf);
  return 0;
}