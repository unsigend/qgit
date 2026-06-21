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

#include "error.h"
#include "obj/object.h"

static int check_type(enum obj_type type)
{
  if (type != OBJ_COMMIT && type != OBJ_BLOB && type != OBJ_TREE &&
      type != OBJ_TAG) {
    setqerrno(QE_INVALIDOBJ);
    return -1;
  }
  return 0;
}

struct obj *obj_create(unsigned char *buf, size_t buflen, enum obj_type type)
{
  if (buflen && !buf)
    return NULL;
  if (check_type(type) == -1)
    return NULL;

  struct obj *obj = NULL;
  size_t payloadsz = buflen + 1; /* always null terminated */

  if (!(obj = calloc(1, sizeof(struct obj))))
    return NULL;
  obj->type = OBJ_NONE;

  if (!(obj->payload = malloc(payloadsz))) {
    obj_close(obj);
    return NULL;
  }

  if (buflen)
    memcpy(obj->payload, buf, buflen);
  ((char *)obj->payload)[buflen] = '\0';
  obj->payloadsz = buflen;
  obj->type = type;
  return obj;
}