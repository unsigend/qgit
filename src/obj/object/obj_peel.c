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

#include <string.h>

#include "error.h"
#include "obj/object.h"

struct obj *obj_peel(struct repo *repo, struct obj *obj, enum obj_type want)
{
  if (!repo || !obj)
    return NULL;

  enum obj_type cur = obj->type;

  if (cur == want)
    return obj;

  if (cur == OBJ_TAG) {
    if (!obj->parsed && obj_parse(obj) == -1) /* make sure the tag is parsed */
      return NULL;

    const char *type = obj_type_to_str(want);
    if (!type)
      return NULL;

    if (strcmp(obj->tag.type, type) == 0)
      return obj_open(repo, obj->tag.object);

    struct obj *next = obj_open(repo, obj->tag.object);
    if (!next)
      return NULL;
    struct obj *peeled = obj_peel(repo, next, want);
    if (!peeled) {
      obj_close(next);
      return NULL;
    }
    obj_close(next);
    return peeled;

  } else if (cur == OBJ_COMMIT) {
    if (want == OBJ_TREE) {
      if (!obj->parsed && obj_parse(obj) == -1)
        return NULL;
      return obj_open(repo, obj->commit.tree);
    } else {
      setqerrno(QE_PEEL);
      return NULL;
    }
  } else if (cur == OBJ_BLOB || cur == OBJ_TREE) {
    setqerrno(QE_PEEL);
    return NULL;
  } else {
    setqerrno(QE_INVALIDOBJ);
    return NULL;
  }

  return NULL;
}