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

#include <time.h>

#include "obj/object.h"
#include "obj/tag.h"
#include "sha1.h"

int tag_fprintf(struct obj *obj, FILE *fp)
{
  if (!obj || !fp)
    return -1;

  unsigned char hex[SHA1_HEXLEN];

  if (sha1_to_hex(obj->tag.object, hex) == -1)
    return -1;

  if (fprintf(fp, "object %s\n", hex) < 0)
    return -1;
  if (fprintf(fp, "type %s\n", obj->tag.type) < 0)
    return -1;
  if (fprintf(fp, "tag %s\n", obj->tag.name) < 0)
    return -1;
  if (fprintf(fp, "tagger %s %ld %s\n", obj->tag.tagger, obj->tag.time,
              obj->tag.timezone) < 0)
    return -1;

  if (fputc('\n', fp) < 0)
    return -1;
  if (obj->tag.msg) {
    if (fprintf(fp, "%s", obj->tag.msg) < 0)
      return -1;
  }

  return 0;
}