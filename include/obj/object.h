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

#ifndef OBJ_OBJECT_H
#define OBJ_OBJECT_H

#include <stddef.h>
#include <stdio.h>

#include "obj/blob.h"
#include "obj/commit.h"
#include "obj/tag.h"
#include "obj/tree.h"
#include "repo.h"
#include "sha1.h"

enum obj_type {
  OBJ_COMMIT,
  OBJ_BLOB,
  OBJ_TREE,
  OBJ_TAG,
  OBJ_NONE = -1,
};

/* qgit object model: asymmetric read/write object model, optimized for reads
   with zero copy.
   Read path: after parse, parsed is set and subtype heap data is owned by the
   union.
   Write path: parsed is unset, only payload is owned by struct obj. Free
   function will only free the payload.

   delegate functions with polymorphic design philosophy: obj_close,
   obj_fprintf, obj_parse
*/

struct obj {
  size_t payloadsz;
  void *payload;
  enum obj_type type;
  unsigned char sha1[SHA1_DIGLEN];
  unsigned parsed : 1;
  union {
    struct commit commit;
    struct blob blob;
    struct tree tree;
    struct tag tag;
  };
};

extern struct obj *obj_open(struct repo *repo, unsigned char *sha1);
extern struct obj *obj_find(struct repo *repo, const char *name,
                            enum obj_type want);
extern struct obj *obj_peel(struct repo *repo, struct obj *obj,
                            enum obj_type want);

extern int obj_parse(struct obj *obj);
extern int obj_fprintf(struct obj *obj, FILE *fp);

extern struct obj *obj_create(unsigned char *buf, size_t buflen,
                              enum obj_type type);
extern int obj_write(struct obj *obj, struct repo *repo);

extern void obj_close(struct obj *obj);

extern enum obj_type obj_type_from_str(const char *str);
extern const char *obj_type_to_str(enum obj_type type);

#endif