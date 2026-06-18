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

int commit_iter_init(struct commit_iter *iter, struct repo *repo,
                     struct obj *start, enum commit_walk_t type)
{
  if (!iter || !repo || !start || start->type != OBJ_COMMIT)
    return -1;

  iter->cur = start;
  iter->type = type;
  iter->repo = repo;

  if (obj_parse_payload(iter->cur) == -1) {
    obj_close(iter->cur);
    return -1;
  }

  if (iter->type == COMMIT_WALK_ALL) {
    /* TODO */
  }
  return 0;
}