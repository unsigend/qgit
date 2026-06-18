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

#include "obj/commit.h"
#include "obj/object.h"

static int iter_inc_first(struct commit_iter *iter)
{

  struct obj *next = NULL;
  unsigned char *sha1 = NULL;

  if (slist_empty(&iter->cur->commit.parents))
    return 1;

  sha1 = slist_front(&iter->cur->commit.parents);
  if (!sha1)
    return -1;
  next = obj_open(iter->repo, sha1);
  if (!next)
    return -1;

  if (obj_parse_payload(next) == -1) {
    obj_close(next);
    return -1;
  }

  obj_close(iter->cur);
  iter->cur = next;
  return 0;
}

static int iter_inc_all(struct commit_iter *iter)
{
  /* TODO */
  (void)iter;
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