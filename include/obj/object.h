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

struct obj {
  size_t payloadsz;
  void *payload;
  enum obj_type type;
  unsigned char sha1[SHA1_DIGLEN];
  union {
    struct commit commit;
    struct blob blob;
    struct tree tree;
    struct tag tag;
  };
};

extern struct obj *obj_open(struct repo *repo, unsigned char *sha1);
extern struct obj *obj_find(struct repo *repo, const char *name);

extern int obj_parse(struct obj *obj);
extern void obj_close(struct obj *obj);
extern int obj_write(struct obj *obj, struct repo *repo);

extern int obj_fprintf(struct obj *obj, FILE *fp);

extern enum obj_type obj_type_from_str(const char *str);
extern const char *obj_type_to_str(enum obj_type type);

/* internal functions */

/* open object from payload buffer with raw file content. */
extern struct obj *obj_open_buf(unsigned char *buf, size_t buflen,
                                enum obj_type type);

/* write "type <size>\0<payload>" to buffer. */
extern int obj_write_buf(struct obj *obj, void **buf, size_t *buflen);

/* store "type <size>\0<payload>" to repository. */
extern int obj_store(struct repo *repo, const unsigned char *sha1,
                     const void *buf, size_t buflen);

#endif