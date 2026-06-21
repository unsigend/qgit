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

#include <stddef.h>
#include <stdlib.h>

#include "obj/object.h"
#include "obj/tag.h"

/* format payload to buffer, for dry run mode ignore buf and buflen */
int fmt_payload(const struct tag *tag, void *buf, size_t buflen)
{
  if (!tag)
    return -1;

  int n = 0;
  unsigned char hex[SHA1_HEXLEN];
  if (sha1_to_hex(tag->object, hex) == -1)
    return -1;

  if (buf && buflen) {
    n = snprintf(buf, buflen,
                 "object %s\ntype %s\ntag %s\ntagger %s <%s> %ld %s\n\n%s", hex,
                 tag->type, tag->name, tag->tagger.name, tag->tagger.email,
                 tag->tagger.time, tag->tagger.zone, tag->msg ? tag->msg : "");

    if (n < 0 || (size_t)n >= buflen)
      return -1;
    else
      return n;

  } else /* dry run mode */
  {
    if ((n = snprintf(
             NULL, 0, "object %s\ntype %s\ntag %s\ntagger %s <%s> %ld %s\n\n%s",
             hex, tag->type, tag->name, tag->tagger.name, tag->tagger.email,
             tag->tagger.time, tag->tagger.zone, tag->msg ? tag->msg : "")) < 0)
      return -1;
    else
      return n;
  }
  return n;
}

struct obj *tag_create(const struct tag *tag)
{
  if (!tag || !tag->type || !tag->name)
    return NULL;
  if (!tag->tagger.name || !tag->tagger.email || !tag->tagger.zone)
    return NULL;

  struct obj *obj = NULL;
  int payloadsz = 0;
  void *payload = NULL;

  if ((payloadsz = fmt_payload(tag, NULL, 0)) < 0)
    return NULL;

  if (!(payload = malloc(payloadsz + 1)))
    return NULL;

  if (fmt_payload(tag, payload, payloadsz + 1) < 0) {
    free(payload);
    return NULL;
  }

  if (!(obj = obj_create(payload, payloadsz, OBJ_TAG))) {
    free(payload);
    return NULL;
  }

  free(payload);
  return obj;
}