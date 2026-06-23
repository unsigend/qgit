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

// #include <limits.h>

// #include "obj/object.h"
// #include "ref.h"
// #include "repo.h"
// #include "rev.h"

// struct obj *obj_find(struct repo *repo, const char *name, enum obj_type want)
// {
//   if (!repo || !name)
//     return NULL;

//   char base[NAME_MAX];
//   enum obj_type peel;
//   enum rev_peel_mode mode;
//   unsigned char sha1[SHA1_DIGLEN];
//   struct obj *obj = NULL, *peeled = NULL;

//   if (rev_parse(name, base, &peel, &mode) == -1)
//     return NULL;

//   if (ref_resolve(repo, base, sha1) == -1)
//     return NULL;

//   if (!(obj = obj_open(repo, sha1)))
//     return NULL;

//   if (mode == REV_PEEL_TO) {
//     want = peel; /* suffix have higher precedence than want */

//     if (want == obj->type)
//       return obj;

//     if (obj_parse(obj) == -1) {
//       obj_close(obj);
//       return NULL;
//     }

//     if (!(peeled = obj_peel(repo, obj, want))) {
//       obj_close(obj);
//       return NULL;
//     }
//     if (peeled != obj)
//       obj_close(obj);
//     return peeled;

//   } else if (mode == REV_PEEL_DEREF) /* dereference tags */
//   {
//     while (obj->type == OBJ_TAG) {
//       if (!obj->parsed && obj_parse(obj) == -1) {
//         obj_close(obj);
//         return NULL;
//       }
//       if (!(peeled = obj_open(repo, obj->tag.object))) {
//         obj_close(obj);
//         return NULL;
//       }
//       obj_close(obj);
//       obj = peeled;
//     }
//     return obj;

//   } else /* REV_PEEL_NONE */
//   {
//     if (want == OBJ_NONE || want == obj->type)
//       return obj;

//     if (obj_parse(obj) == -1) {
//       obj_close(obj);
//       return NULL;
//     }
//     if (!(peeled = obj_peel(repo, obj, want))) {
//       obj_close(obj);
//       return NULL;
//     }
//     if (peeled != obj)
//       obj_close(obj);
//     return peeled;
//   }
// }

#include <limits.h>

#include "obj/object.h"
#include "ref.h"
#include "repo.h"
#include "rev.h"

struct obj *obj_find(struct repo *repo, const char *name, enum obj_type want)
{
  if (!repo || !name)
    return NULL;

  char base[NAME_MAX];
  enum obj_type peel;
  enum rev_peel_mode mode;
  unsigned char sha1[SHA1_DIGLEN];
  struct obj *obj = NULL, *peeled = NULL, *cur = NULL;

  if (rev_parse(name, base, &peel, &mode) == -1)
    return NULL;

  if (ref_resolve(repo, base, sha1) == -1)
    return NULL;

  if (!(obj = obj_open(repo, sha1)))
    return NULL;

  if (mode == REV_PEEL_TO) {
    if (peel != obj->type) {
      if (obj_parse(obj) == -1) {
        obj_close(obj);
        return NULL;
      }

      if (!(peeled = obj_peel(repo, obj, peel))) {
        obj_close(obj);
        return NULL;
      }

      if (peeled != obj)
        obj_close(obj);
    }
  } else if (mode == REV_PEEL_DEREF) /* dereference tags */
  {
    while (obj->type == OBJ_TAG) {
      if (obj_parse(obj) == -1) {
        obj_close(obj);
        return NULL;
      }

      if (!(peeled = obj_open(repo, obj->tag.object))) {
        obj_close(obj);
        return NULL;
      }
      obj_close(obj);
      obj = peeled;
    }
  }

  cur = peeled ? peeled : obj;
  if (cur->type == want || want == OBJ_NONE)
    return cur;

  if (obj_parse(cur) == -1) {
    obj_close(cur);
    return NULL;
  }

  struct obj *final = obj_peel(repo, cur, want);
  if (!final) {
    obj_close(cur);
    return NULL;
  }

  if (final != cur)
    obj_close(cur);
  return final;
}