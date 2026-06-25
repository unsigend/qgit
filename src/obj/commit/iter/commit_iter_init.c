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

#include "obj/commit.h"
#include "obj/object.h"

/* priority queue store struct obj* and compare by commit time */
static void pq_destroy(void *obj) { obj_close(*(struct object **)obj); };
static int pq_cmp(void *lhs, void *rhs)
{
  struct object *obj1 = *(struct object **)lhs;
  struct object *obj2 = *(struct object **)rhs;
  struct sign *comsign = &obj1->commit.committer; /* committer sign */
  if (comsign->time > obj2->commit.committer.time)
    return -1;
  else if (comsign->time < obj2->commit.committer.time)
    return 1;
  else
    return 0;
}

static uint32_t set_hash(void *sha1) { return *(uint32_t *)sha1; }
static int set_cmp(void *lhs, void *rhs)
{
  return memcmp(lhs, rhs, SHA1_DIGLEN);
}

int commit_iter_init(struct commit_iter *iter, struct repo *repo,
                     struct object *start, enum commit_walk_t type)
{
  if (!iter || !repo || !start || start->type != OBJ_COMMIT)
    return -1;

  iter->cur = start;
  iter->type = type;
  iter->repo = repo;

  if (iter->type == COMMIT_WALK_ALL) {
    if (heap_init(&iter->pq, sizeof(struct object *), pq_cmp, pq_destroy) ==
        -1) {
      obj_close(iter->cur);
      return -1;
    }

    iter->fns.destroy = free;
    iter->fns.hash = set_hash;
    iter->fns.cmp = set_cmp;

    if (set_init(&iter->visited, &iter->fns) == -1) {
      heap_fini(&iter->pq);
      obj_close(iter->cur);
      return -1;
    }

    unsigned char *sha1 = sha1dup(iter->cur->sha1);
    if (!sha1) {
      heap_fini(&iter->pq);
      set_fini(&iter->visited);
      obj_close(iter->cur);
      return -1;
    }

    if (set_insert(&iter->visited, sha1) == -1) {
      free(sha1);
      heap_fini(&iter->pq);
      set_fini(&iter->visited);
      obj_close(iter->cur);
      return -1;
    }
  }
  return 0;
}