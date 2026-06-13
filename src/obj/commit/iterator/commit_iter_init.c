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

#include "obj/commit.h"
#include "obj/obj.h"

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

  return 0;
}