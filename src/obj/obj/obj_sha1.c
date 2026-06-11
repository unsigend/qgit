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

#include "obj/obj.h"

int obj_sha1(struct obj *obj)
{
  if (!obj) {
    errno = EINVAL;
    return -1;
  }

  size_t rawbuflen;
  void *rawbuf = obj_build_rawbuf(obj, &rawbuflen);
  if (!rawbuf)
    return -1;

  if (sha1(rawbuf, rawbuflen, obj->sha1) == -1) {
    free(rawbuf);
    return -1;
  }

  free(rawbuf);
  return 0;
}