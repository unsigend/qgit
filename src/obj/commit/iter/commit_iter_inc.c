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

#include "collection/vector.h"
#include "obj/commit.h"
#include "obj/object.h"

static int iter_inc_first(struct commit_iter *iter)
{

  struct object *next = NULL;
  unsigned char *sha1 = NULL;

  if (slist_empty(&iter->cur->commit.parents))
    return 1;

  sha1 = slist_front(&iter->cur->commit.parents);
  if (!sha1)
    return -1;
  next = obj_open(iter->repo, sha1);
  if (!next)
    return -1;

  if (obj_parse(next) == -1) {
    obj_close(next);
    return -1;
  }

  obj_close(iter->cur);
  iter->cur = next;
  return 0;
}

static int iter_inc_all(struct commit_iter *iter)
{
  struct object *next = NULL;
  unsigned char *sha1 = NULL;
  struct slist_iter it;
  struct object *obj = NULL;

  if (!slist_empty(&iter->cur->commit.parents)) {
    if (slist_iter_init(&it, &iter->cur->commit.parents) == -1)
      return -1;

    while (slist_iter_get(&it)) {
      sha1 = sha1dup(slist_iter_get(&it));
      if (!sha1)
        return -1;
      if (set_contains(&iter->visited, sha1)) {
        free(sha1);
        slist_iter_inc(&it);
        continue;
      }

      obj = obj_open(iter->repo, sha1);
      if (!obj) {
        free(sha1);
        return -1;
      }

      if (obj_parse(obj) == -1) {
        obj_close(obj);
        free(sha1);
        return -1;
      }

      if (set_insert(&iter->visited, sha1) == -1) {
        obj_close(obj);
        free(sha1);
        return -1;
      }

      if (heap_push(&iter->pq, &obj) == -1) {
        set_remove(&iter->visited, sha1);
        obj_close(obj);
        return -1;
      }

      slist_iter_inc(&it);
    }
  }

  if (heap_empty(&iter->pq))
    return 1;

  if (heap_pop(&iter->pq, &next) == -1)
    return -1;

  obj_close(iter->cur);
  iter->cur = next;
  return 0;
}

int commit_iter_inc(struct commit_iter *iter)
{
  if (!iter)
    return -1;

  if (iter->type == COMMIT_WALK_FPARENT) {
    return iter_inc_first(iter);
  } else if (iter->type == COMMIT_WALK_ALL) {
    return iter_inc_all(iter);
  } else
    return -1;
}