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

int obj_write_buf(struct obj *obj, void **buf, size_t *buflen)
{
  if (!obj || !buf || !buflen)
    return -1;

  const char *type = NULL;
  char *cur = NULL;
  int n = 0;

  type = obj_type_to_str(obj->type);
  if (!type)
    return -1;

  if ((n = snprintf(NULL, 0, "%s %zu", type, obj->payloadsz)) < 0)
    return -1;

  *buflen = n + obj->payloadsz + 1;
  if (!(*buf = malloc(*buflen)))
    return -1;

  cur = (char *)*buf;

  if ((n = (snprintf(cur, *buflen, "%s %zu", type, obj->payloadsz))) < 0) {
    free(*buf);
    *buf = NULL;
    return -1;
  }

  cur += n;
  *cur++ = '\0';
  if (obj->payload && obj->payloadsz)
    memcpy(cur, obj->payload, obj->payloadsz);
  return 0;
}