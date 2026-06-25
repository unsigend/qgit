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

#include "collection/vector.h"
#include "error.h"
#include "obj/object.h"
#include "obj/tree.h"
#include "sha1.h"

static char *parse_entry(char *buf, char *end, struct tree_entry *entry)
{
  char *endstr = NULL;
  char *cur = buf;

  errno = 0;
  entry->mode = strtoul(cur, &endstr, 8);
  if (errno || endstr == cur || endstr >= end || *endstr != ' ') {
    if (!errno)
      setqerrno(QE_BADOBJFILE);
    return NULL;
  }
  cur = endstr + 1;
  entry->path = cur;

  while (cur < end && *cur)
    cur++;
  if (cur == end) {
    setqerrno(QE_BADOBJFILE);
    return NULL;
  }
  cur++; /* skip '\0' */
  if (cur + SHA1_DIGLEN > end) {
    setqerrno(QE_BADOBJFILE);
    return NULL;
  }

  sha1_copy((unsigned char *)cur, entry->sha1);
  cur += SHA1_DIGLEN;
  return cur;
}

int tree_parse(struct object *obj)
{
  if (!obj || obj->type != OBJ_TREE)
    return -1;

  char *cur = obj->payload;
  char *end = cur + obj->payloadsz;

  if (vec_init(&obj->tree.entries, sizeof(struct tree_entry), NULL) == -1)
    return -1;

  while (cur < end) {
    struct tree_entry entry;
    cur = parse_entry(cur, end, &entry);
    if (!cur)
      return -1;
    if (vec_pushback(&obj->tree.entries, &entry) == -1)
      return -1;
  }
  return 0;
}