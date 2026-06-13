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

#include "collection/slist.h"
#include "obj/commit.h"
#include "obj/obj.h"

static int iter_inc_first(struct commit_iter *iter)
{
  struct obj *next;
  struct commit *commit = &iter->cur->commit;
  unsigned char *sha1;

  if (slist_empty(&commit->parents))
    return 1;

  sha1 = slist_front(&commit->parents);
  next = obj_open_sha1(iter->repo, sha1);
  if (!next)
    return -1;

  if (commit_parse(next) == -1) {
    obj_close(next);
    return -1;
  }

  /* Close the current commit only if can move forward to next commit */
  obj_close(iter->cur);
  iter->cur = next;

  return 0;
}

// static int iter_inc_all(struct commit_iter *iter)
// {
//   /* TODO: Implement */
//   return 0;
// }

int commit_iter_inc(struct commit_iter *iter)
{
  if (!iter) {
    errno = EINVAL;
    return -1;
  }
  switch (iter->type) {
  case COMMIT_WALK_FIRST:
    return iter_inc_first(iter);
  case COMMIT_WALK_ALL:
    // return iter_inc_all(iter);
    return 0;
  default:
    errno = EINVAL;
    return -1;
  }
}