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

static char *parse_author(char *cur, char *end, const char **author,
                          time_t *time, const char **zone)
{
  *author = cur;
  while (cur < end && *cur != '>')
    cur++;
  if (cur >= end)
    return NULL;
  *(++cur) = '\0';
  if (cur >= end)
    return NULL;

  const char *s = ++cur;
  while (cur < end && *cur != ' ')
    cur++;
  if (cur >= end)
    return NULL;
  *cur++ = '\0';
  if (cur >= end)
    return NULL;
  *zone = cur;
  while (cur < end && *cur != '\n')
    cur++;
  *cur++ = '\0';
  char *e = NULL;
  errno = 0;
  *time = (time_t)strtol(s, &e, 10);
  if (*e != '\0' || errno != 0)
    *time = 0;
  return cur;
}

static int parse_head(char *cur, char *end, struct commit *commit)
{
  while (cur < end) {
    if (*cur == '\n') {
      cur++;
      continue;
    }
    if (strncmp(cur, "tree", 4) == 0) {
      cur += 5;
      if (hex_to_sha1((unsigned char *)cur, commit->tree) == -1)
        return -1;
      cur += SHA1_HEX_LENGTH - 1;
    } else if (strncmp(cur, "parent", 6) == 0) {
      cur += 7;
      unsigned char *sha1 = malloc(SHA1_DIGEST_LENGTH);
      if (!sha1)
        return -1;
      if (hex_to_sha1((unsigned char *)cur, sha1) == -1) {
        free(sha1);
        return -1;
      }
      if (slist_pushback(&commit->parents, sha1) == -1) {
        free(sha1);
        return -1;
      }
      cur += SHA1_HEX_LENGTH - 1;
    } else if (strncmp(cur, "author", 6) == 0) {
      if (commit->author)
        return -1;
      cur += 7;
      cur = parse_author(cur, end, &commit->author, &commit->atime,
                         &commit->azone);
      if (!cur)
        return -1;
    } else if (strncmp(cur, "committer", 9) == 0) {
      if (commit->committer)
        return -1;
      cur += 10;
      cur = parse_author(cur, end, &commit->committer, &commit->ctime,
                         &commit->czone);
      if (!cur)
        return -1;
    }
  }
  return 0;
}

/* Assume the raw payload is valid layout, since only qgit and git can create
   commit objects */
int commit_parse(struct obj *obj)
{
  if (!obj) {
    errno = EINVAL;
    return -1;
  }

  struct commit *commit = &obj->commit;
  memset(commit, 0, sizeof(struct commit));

  if (slist_init(&commit->parents, free) == -1)
    return -1;

  char *cur = obj->payload;
  char *body = strstr(cur, "\n\n");
  if (body)
    commit->msg = body + 2;
  else
    body = cur + obj->payloadsz;

  if (parse_head(cur, body, commit) == -1) {
    slist_fini(&commit->parents);
    return -1;
  }

  return 0;
}
