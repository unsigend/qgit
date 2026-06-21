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

#include "collection/slist.h"
#include "error.h"
#include "obj/commit.h"
#include "obj/object.h"
#include "sha1.h"

static int parse_header(struct obj *obj, char *cur, char *end)
{
  int has_tree = 0;
  while (cur < end) {
    if (*cur == '\n') {
      cur++;
      continue;
    }
    if (strncmp(cur, "tree ", 5) == 0) {
      if (has_tree) {
        setqerrno(QE_BADOBJFILE);
        return -1;
      }
      has_tree = 1;
      cur += 5;
      if (cur + SHA1_HEXLEN >= end) {
        setqerrno(QE_BADOBJFILE);
        return -1;
      }
      if (hex_to_sha1((unsigned char *)cur, obj->commit.tree) == -1) {
        setqerrno(QE_BADOBJFILE);
        return -1;
      }
      cur += SHA1_HEXLEN;

    } else if (strncmp(cur, "parent ", 7) == 0) {
      cur += 7;
      if (cur + SHA1_HEXLEN >= end) {
        setqerrno(QE_BADOBJFILE);
        return -1;
      }
      void *sha1 = malloc(SHA1_DIGLEN);
      if (!sha1)
        return -1;

      if (hex_to_sha1((unsigned char *)cur, sha1) == -1) {
        free(sha1);
        return -1;
      }

      if (slist_pushback(&obj->commit.parents, sha1) == -1) {
        free(sha1);
        return -1;
      }
      cur += SHA1_HEXLEN;
    } else if (strncmp(cur, "author ", 7) == 0) {
      if (obj->commit.author.name) {
        setqerrno(QE_BADOBJFILE);
        return -1;
      }
      cur += 7;
      cur = sign_parse(&obj->commit.author, cur, end);
      if (!cur)
        return -1;
    } else if (strncmp(cur, "committer ", 10) == 0) {
      if (obj->commit.committer.name) {
        setqerrno(QE_BADOBJFILE);
        return -1;
      }
      cur += 10;
      cur = sign_parse(&obj->commit.committer, cur, end);
      if (!cur)
        return -1;
    } else {
      setqerrno(QE_BADOBJFILE);
      return -1;
    }
  }
  if (!has_tree || !obj->commit.author.name || !obj->commit.committer.name) {
    setqerrno(QE_BADOBJFILE);
    return -1;
  }
  return 0;
}

int commit_parse(struct obj *obj)
{
  if (!obj || obj->type != OBJ_COMMIT)
    return -1;

  char *cur = obj->payload;
  char *body = NULL, *end;

  if (slist_init(&obj->commit.parents, free) == -1)
    return -1;

  body = strstr(cur, "\n\n");
  if (body)
    end = body + 1;
  else
    end = cur + obj->payloadsz;

  if (parse_header(obj, cur, end) == -1) {
    slist_fini(&obj->commit.parents);
    return -1;
  }
  if (body)
    obj->commit.msg = body + 2;
  else
    obj->commit.msg = NULL;
  return 0;
}