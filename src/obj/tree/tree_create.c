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

#include "collection/vector.h"
#include "obj/object.h"
#include "obj/tree.h"
#include "sha1.h"

static int tree_entry_cmp(const void *a, const void *b)
{
  const struct tree_entry *e1 = (const struct tree_entry *)a;
  const struct tree_entry *e2 = (const struct tree_entry *)b;
  const char *p1 = e1->path, *p2 = e2->path;

  while (*p1 && *p2 && *p1 == *p2)
    p1++, p2++;

  char c1 = *p1 ? *p1 : (S_ISDIR(e1->mode) ? '/' : '\0');
  char c2 = *p2 ? *p2 : (S_ISDIR(e2->mode) ? '/' : '\0');
  return (unsigned char)c1 - (unsigned char)c2;
}

static int fmt_entry(const struct tree_entry *entry, void *buf, size_t buflen)
{
  if (!entry)
    return -1;
  int n = 0, sz = 0;

  if ((n = snprintf(buf, buflen, "%o %s", entry->mode, entry->path)) < 0)
    return -1;

  sz += n;
  if (buf && buflen)
    ((char *)buf)[sz] = '\0';
  sz += 1;

  if (buf && buflen && sz + SHA1_DIGLEN <= (int)buflen)
    sha1_copy(entry->sha1, (unsigned char *)buf + sz);
  sz += SHA1_DIGLEN;

  if (buflen && sz >= (int)buflen)
    return -1;
  return sz;
}

static int fmt_tree(const struct tree *tree, void *buf, size_t buflen)
{
  int n = 0, sz = 0;
  struct vector_iter it;

  vec_sort((struct vector *)&tree->entries, tree_entry_cmp);

  if (vec_iter_init(&it, (struct vector *)&tree->entries) == -1)
    return -1;

  while (vec_iter_get(&it)) {
    struct tree_entry *entry = vec_iter_get(&it);
    if ((n = fmt_entry(entry, buf ? buf + sz : NULL,
                       buflen ? buflen - sz : 0)) < 0)
      return -1;
    sz += n;
    vec_iter_inc(&it);
  }

  if (buflen && sz >= (int)buflen)
    return -1;
  return sz;
}

struct object *tree_create(const struct tree *tree)
{
  if (!tree)
    return NULL;

  struct object *obj = NULL;
  int payloadsz = 0;
  void *payload = NULL;

  if ((payloadsz = fmt_tree(tree, NULL, 0)) < 0)
    return NULL;

  if (!(payload = malloc(payloadsz + 1)))
    return NULL;

  if (fmt_tree(tree, payload, payloadsz + 1) < 0) {
    free(payload);
    return NULL;
  }

  if (!(obj = obj_create(payload, payloadsz, OBJ_TREE))) {
    free(payload);
    return NULL;
  }

  free(payload);
  return obj;
}