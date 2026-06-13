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
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "obj/commit.h"
#include "obj/obj.h"
#include "sha1.h"

static uint32_t commit_hash(void *sha1) { return *(uint32_t *)sha1; }
static int sha1_cmp(void *lhs, void *rhs)
{
  return memcmp(lhs, rhs, SHA1_DIGEST_LENGTH);
}

static int timestamp_cmp(void *lhs, void *rhs)
{
  struct obj *objlhs = *(struct obj **)lhs;
  struct obj *objrhs = *(struct obj **)rhs;
  if (objlhs->commit.atime > objrhs->commit.atime)
    return -1;
  else if (objlhs->commit.atime < objrhs->commit.atime)
    return 1;
  else
    return 0;
}

static void destroy(void *obj) { obj_close(*(struct obj **)obj); }

int commit_iter_init(struct commit_iter *iter, struct obj *start,
                     struct repo *repo, commit_walk_type_t type)
{
  if (!iter || !start || !repo) {
    errno = EINVAL;
    return -1;
  }

  iter->cur = start;
  if (obj_parse(start) == -1)
    return -1;
  iter->repo = repo;
  iter->type = type;

  if (type == COMMIT_WALK_ALL) {

    iter->set_fns.hash = commit_hash;
    iter->set_fns.cmp = sha1_cmp;
    iter->set_fns.destroy = free;

    if (set_init(&iter->visited, &iter->set_fns) == -1)
      return -1;

    if (heap_init(&iter->pq, sizeof(struct obj *), timestamp_cmp, destroy) ==
        -1) {
      set_fini(&iter->visited);
      return -1;
    }
  }

  return 0;
}