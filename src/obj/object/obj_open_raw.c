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
#include <limits.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "obj/object.h"

struct obj *obj_open_raw(unsigned char *buf, size_t buflen)
{
  if (!buf || !buflen)
    return NULL;

  unsigned char *cur = buf;
  unsigned char *end = cur + buflen;
  size_t len, payloadsz;
  char *endstr = NULL;
  enum obj_type type = OBJ_NONE;
  struct obj *obj = NULL;

  while (cur < end && *cur != ' ')
    cur++;

  if (cur == end) {
    setqerrno(QE_BADOBJFILE);
    return NULL;
  }

  len = cur - (unsigned char *)buf;

  if (len == 6 && memcmp((char *)buf, "commit", len) == 0)
    type = OBJ_COMMIT;
  else if (len == 4 && memcmp((char *)buf, "blob", len) == 0)
    type = OBJ_BLOB;
  else if (len == 4 && memcmp((char *)buf, "tree", len) == 0)
    type = OBJ_TREE;
  else if (len == 3 && memcmp((char *)buf, "tag", len) == 0)
    type = OBJ_TAG;
  else {
    setqerrno(QE_BADOBJFILE);
    return NULL;
  }
  cur++;

  errno = 0;
  payloadsz = strtoul((char *)cur, &endstr, 10);
  if (errno || endstr == (char *)cur || *endstr) {
    if (!errno)
      setqerrno(QE_BADOBJFILE);
    return NULL;
  }

  while (cur < end && *cur)
    cur++;

  if (cur == end) {
    setqerrno(QE_BADOBJFILE);
    return NULL;
  }
  cur++;
  if (cur == end && payloadsz) {
    setqerrno(QE_BADOBJFILE);
    return NULL;
  }

  if (!(obj = calloc(1, sizeof(struct obj))))
    return NULL;

  obj->type = type;
  obj->payloadsz = payloadsz;
  obj->payload = malloc(payloadsz + 1); /* always null terminated */
  if (!obj->payload) {
    obj_close(obj);
    return NULL;
  }
  if (payloadsz > (size_t)(end - cur)) {
    obj_close(obj);
    setqerrno(QE_BADOBJFILE);
    return NULL;
  }
  memcpy(obj->payload, cur, payloadsz);
  ((char *)obj->payload)[payloadsz] = '\0';
  return obj;
}