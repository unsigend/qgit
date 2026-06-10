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
#include <stdlib.h>
#include <string.h>

#include "obj/obj.h"

void *obj_build_rawbuf(struct obj *obj, size_t *buflen)
{
  if (!obj || !buflen) {
    errno = EINVAL;
    return NULL;
  }

  const char *type = str_from_obj_type(obj->type);
  if (!type)
    return NULL;
  int nw = 0;
  nw = snprintf(NULL, 0, "%zu", obj->payloadsz);
  if (nw < 0) {
    errno = EINVAL;
    return NULL;
  }

  *buflen = strlen(type) + 2 + nw + obj->payloadsz;
  void *buf = malloc(*buflen);
  if (!buf)
    return NULL;

  /* obj format: <type> <payloadsz>\0<payload> */
  unsigned char *cursor = (unsigned char *)buf;
  if ((nw = snprintf(buf, *buflen, "%s %zu", type, obj->payloadsz)) < 0) {
    free(buf);
    return NULL;
  }
  cursor += nw;
  *cursor++ = '\0';
  if (obj->payloadsz > 0)
    memcpy(cursor, obj->payload, obj->payloadsz);

  return buf;
}