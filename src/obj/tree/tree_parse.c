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

#include "collection/vector.h"
#include "obj/obj.h"
#include "obj/tree.h"

static char *parse_entry(char *cur, char *end, struct tree_entry *entry)
{
  if (!cur || !end || !entry) {
    errno = EINVAL;
    return NULL;
  }

  entry->mode = cur;
  while (cur < end && *cur != ' ')
    cur++;
  if (cur >= end)
    return NULL;
  *cur++ = '\0';
  entry->path = cur;
  while (cur < end && *cur)
    cur++;
  if (cur >= end)
    return NULL;
  cur++; /* skip '\0' */
  if (cur >= end)
    return NULL;
  if (cur + SHA1_DIGEST_LENGTH > end)
    return NULL;
  sha1_copy((unsigned char *)cur, entry->sha1);
  return cur + SHA1_DIGEST_LENGTH; /* skip sha1 */
}

int tree_parse(struct obj *obj)
{
  if (!obj) {
    errno = EINVAL;
    return -1;
  }

  struct tree *tree = &obj->tree;
  if (vec_init(&tree->entries, sizeof(struct tree_entry), NULL) == -1)
    return -1;

  char *cursor = obj->payload;
  char *end = cursor + obj->payloadsz;

  while (cursor < end) {
    struct tree_entry entry;
    memset(&entry, 0, sizeof(struct tree_entry));

    cursor = parse_entry(cursor, end, &entry);
    if (!cursor) {
      tree_free(tree);
      return -1;
    }
    if (vec_pushback(&tree->entries, &entry) == -1) {
      tree_free(tree);
      return -1;
    }
  }

  return 0;
}