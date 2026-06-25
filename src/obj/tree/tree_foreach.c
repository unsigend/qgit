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
#include <limits.h>

#include "obj/object.h"
#include "obj/tree.h"

static int foreach (struct object *obj, struct repo * repo, tree_foreach_cb cb,
                    void *arg, const char *prefix)
{
  struct vector_iter iter;
  char buf[PATH_MAX];
  int ret = 0;

  if (vec_iter_init(&iter, &obj->tree.entries) == -1)
    return -1;

  while (vec_iter_get(&iter)) {
    struct tree_entry *entry = vec_iter_get(&iter);
    if (cb(entry, prefix, arg) == -1)
      return -1;

    if (S_ISDIR(entry->mode)) /* recursive to subtree */
    {
      obj = obj_open(repo, entry->sha1);
      if (!obj)
        return -1;
      if (obj->type != OBJ_TREE) {
        obj_close(obj);
        return -1;
      }
      if (obj_parse(obj) == -1) {
        obj_close(obj);
        return -1;
      }
      if (snprintf(buf, PATH_MAX, "%s%s%s", prefix ? prefix : "",
                   prefix ? "/" : "", entry->path) >= PATH_MAX) {
        errno = ENAMETOOLONG;
        obj_close(obj);
        return -1;
      }
      ret = foreach (obj, repo, cb, arg, buf);
      if (ret != 0) {
        obj_close(obj);
        return ret;
      }
      obj_close(obj);
    }
    vec_iter_inc(&iter);
  }
  return 0;
}

int tree_foreach(struct object *obj, struct repo *repo, tree_foreach_cb cb,
                 void *arg)
{
  if (!obj || !repo || !cb || obj->type != OBJ_TREE)
    return -1;

  return foreach (obj, repo, cb, arg, NULL);
}