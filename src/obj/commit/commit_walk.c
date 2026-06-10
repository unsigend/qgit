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

static int walk_first(struct obj *obj, struct repo *repo, commit_walk_cb cb,
                      void *arg)
{
  int ret = cb(obj, arg);
  if (ret == -1)
    return -1;
  if (ret == 1)
    return 0;

  struct slist *parents = obj->commit.parents;
  int has_next = parents && !slist_empty(parents);
  if (!has_next)
    return 0;

  unsigned char next_sha1[SHA1_DIGEST_LENGTH];
  sha1_copy(slist_front(parents), next_sha1);

  while (has_next) {
    obj = obj_open_sha1(repo, next_sha1);
    if (!obj)
      return -1;
    if (obj->type != OBJ_COMMIT) {
      obj_close(obj);
      return -1;
    }
    if (obj_parse(obj) == -1) {
      obj_close(obj);
      return -1;
    }

    ret = cb(obj, arg);
    if (ret == -1) {
      obj_close(obj);
      return -1;
    }

    if (ret == 1) {
      obj_close(obj);
      return 0;
    }

    parents = obj->commit.parents;
    has_next = parents && !slist_empty(parents);

    if (has_next)
      sha1_copy(slist_front(parents), next_sha1);
    obj_close(obj);
  }

  return 0;
}

int commit_walk(struct obj *obj, commit_walk_type_t type, struct repo *repo,
                commit_walk_cb cb, void *arg)
{
  if (!obj || !cb || !repo) {
    errno = EINVAL;
    return -1;
  }

  switch (type) {
  case COMMITWK_FIRST: {
    return walk_first(obj, repo, cb, arg);
  }
  case COMMITWK_ALL: {
    /* TODO: BFS walk with a hashtbl to avoid visited */
    break;
  }
  default: {
    errno = EINVAL;
    return -1;
  }
  }

  return 0;
}
