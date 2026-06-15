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
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "collection/vector.h"
#include "obj/obj.h"
#include "obj/tree.h"

static int traverse(struct obj *obj, tree_traverse_cb cb, const char *prefix,
                    struct repo *repo, void *arg)
{
  for (size_t i = 0; i < vec_size(&obj->tree.entries); i++) {
    struct tree_entry *entry = vec_at(&obj->tree.entries, i);
    if (!entry) {
      errno = EINVAL;
      return -1;
    }
    if (cb(entry, prefix, arg) == -1)
      return -1;
    if (S_ISDIR(entry->mode)) {
      struct obj *subobj = obj_open_sha1(repo, entry->sha1);
      if (!subobj)
        return -1;
      if (obj_parse(subobj) == -1) {
        obj_close(subobj);
        return -1;
      }

      char *buf = NULL;
      if (prefix) {
        size_t len = strlen(prefix) + strlen(entry->path) + 2;
        buf = malloc(len); /* use heap to avoid stack overflow in small stack */
        if (!buf) {
          obj_close(subobj);
          return -1;
        }
        if (snprintf(buf, len, "%s/%s", prefix, entry->path) >= (int)len) {
          free(buf);
          obj_close(subobj);
          return -1;
        }
      } else {
        buf = strdup(entry->path);
        if (!buf) {
          obj_close(subobj);
          return -1;
        }
      }
      if (traverse(subobj, cb, buf, repo, arg) == -1) {
        obj_close(subobj);
        free(buf);
        return -1;
      }

      obj_close(subobj);
      free(buf);
    }
  }
  return 0;
}

int tree_traverse(struct obj *obj, tree_traverse_cb cb, struct repo *repo,
                  void *arg)
{
  if (!obj || !cb || !repo) {
    errno = EINVAL;
    return -1;
  }
  return traverse(obj, cb, NULL, repo, arg);
}