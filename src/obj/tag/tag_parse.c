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
#include "obj/tag.h"
#include "sha1.h"

static int parse_header(struct obj *obj, char *cur, char *end)
{
  int has_object = 0;
  while (cur < end) {
    if (*cur == '\n') {
      cur++;
      continue;
    }
    if (strncmp(cur, "object ", 7) == 0) {
      if (has_object) {
        setqerrno(QE_BADOBJFILE);
        return -1;
      }
      has_object = 1;
      cur += 7;
      if (cur + SHA1_HEXLEN >= end) {
        setqerrno(QE_BADOBJFILE);
        return -1;
      }
      if (hex_to_sha1((unsigned char *)cur, obj->tag.object) == -1) {
        setqerrno(QE_BADOBJFILE);
        return -1;
      }
      cur += SHA1_HEXLEN;

    } else if (strncmp(cur, "type ", 5) == 0) {
      if (obj->tag.type) {
        setqerrno(QE_BADOBJFILE);
        return -1;
      }
      cur += 5;
      const char *type = cur;
      while (cur < end && *cur != '\n')
        cur++;
      if (cur == end) {
        setqerrno(QE_BADOBJFILE);
        return -1;
      }
      *cur++ = '\0';
      if (strcmp(type, "commit") == 0) {
        obj->tag.type = "commit";
      } else if (strcmp(type, "tree") == 0) {
        obj->tag.type = "tree";
      } else if (strcmp(type, "blob") == 0) {
        obj->tag.type = "blob";
      } else if (strcmp(type, "tag") == 0) {
        obj->tag.type = "tag";
      } else {
        setqerrno(QE_BADOBJFILE);
        return -1;
      }
    } else if (strncmp(cur, "tag ", 4) == 0) {
      if (obj->tag.name) {
        setqerrno(QE_BADOBJFILE);
        return -1;
      }
      cur += 4;
      obj->tag.name = cur;
      while (cur < end && *cur != '\n')
        cur++;
      *cur++ = '\0';

    } else if (strncmp(cur, "tagger ", 7) == 0) {
      if (obj->tag.tagger.name) {
        setqerrno(QE_BADOBJFILE);
        return -1;
      }
      cur += 7;
      cur = sign_parse(&obj->tag.tagger, cur, end);
      if (!cur)
        return -1;
    } else {
      setqerrno(QE_BADOBJFILE);
      return -1;
    }
  }
  if (!has_object || !obj->tag.type || !obj->tag.name ||
      !obj->tag.tagger.name) {
    setqerrno(QE_BADOBJFILE);
    return -1;
  }
  return 0;
}

int tag_parse(struct obj *obj)
{
  if (!obj || obj->type != OBJ_TAG)
    return -1;

  char *cur = obj->payload;
  char *body = NULL, *end;

  body = strstr(cur, "\n\n");
  if (body)
    end = body + 1;
  else
    end = cur + obj->payloadsz;

  if (parse_header(obj, cur, end) == -1)
    return -1;

  if (body)
    obj->tag.msg = body + 2;
  else
    obj->tag.msg = NULL;
  return 0;
}