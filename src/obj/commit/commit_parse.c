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
#include <string.h>

#include "collection/slist.h"
#include "obj/commit.h"
#include "obj/obj.h"
#include "sha1.h"

static char *parsekv(char *buf, char *end, char **key, char **val)
{
  if (!buf || !end || !key || !val) {
    errno = EINVAL;
    return buf;
  }

  *key = buf;
  while (buf < end && *buf != ' ')
    buf++;
  if (buf < end)
    *buf++ = '\0';
  *val = buf;
  while (buf < end && *buf != '\n')
    buf++;
  if (buf < end)
    *buf++ = '\0';
  return buf;
}

int commit_parse(struct obj *obj)
{
  if (!obj) {
    errno = EINVAL;
    return -1;
  }

  memset(&obj->commit, 0, sizeof(obj->commit));

  char *cursor = obj->payload;
  char *end = cursor + obj->payloadsz;

  while (cursor < end) {
    char *key = NULL, *val = NULL;
    if (*cursor == '\n') {
      cursor++;
      break;
    }
    cursor = parsekv(cursor, end, &key, &val);
    if (strcmp(key, "tree") == 0) {
      if (hex_to_sha1((unsigned char *)val, obj->commit.tree) == -1) {
        commit_free(&obj->commit);
        return -1;
      }
    } else if (strcmp(key, "parent") == 0) {
      if (!obj->commit.parents) {
        obj->commit.parents = calloc(1, sizeof(struct slist));
        if (!obj->commit.parents)
          return -1;
        if (slist_init(obj->commit.parents, free) == -1) {
          free(obj->commit.parents);
          obj->commit.parents = NULL;
          return -1;
        }
      }
      void *parent = malloc(SHA1_DIGEST_LENGTH);
      if (!parent) {
        commit_free(&obj->commit);
        return -1;
      }
      if (hex_to_sha1((unsigned char *)val, (unsigned char *)parent) == -1) {
        free(parent);
        commit_free(&obj->commit);
        return -1;
      }
      if (slist_pushback(obj->commit.parents, parent) == -1) {
        free(parent);
        commit_free(&obj->commit);
        return -1;
      }
    } else if (strcmp(key, "author") == 0)
      obj->commit.author = val;
    else if (strcmp(key, "committer") == 0)
      obj->commit.committer = val;
  }

  if (cursor >= end) {
    commit_free(&obj->commit);
    errno = EINVAL;
    return -1;
  }
  obj->commit.message = cursor;
  return 0;
}