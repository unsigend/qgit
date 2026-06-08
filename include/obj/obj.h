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

#ifndef OBJ_H
#define OBJ_H

#include <stddef.h>

#include "obj/blob.h"
#include "obj/commit.h"
#include "obj/tag.h"
#include "obj/tree.h"
#include "repo.h"

typedef enum {
  OBJ_BLOB,
  OBJ_COMMIT,
  OBJ_TREE,
  OBJ_TAG,
} obj_type_t;

/* qgit object model, the obj struct is the central structure delegate the
   actual payload to the appropriate type. */
struct obj {
  obj_type_t type;
  size_t payloadsz;
  void *payload;
  unsigned char sha1[20];

  union {
    struct blob blob;
    struct commit commit;
    struct tree tree;
    struct tag tag;
  };
};

/* Open an object from the repository by its sha1, return object on success,
   NULL on error and set errno. */
extern struct obj *obj_open(struct repo *repo, const unsigned char *sha1);

/* Write an object to the repository, return 0 on success, -1 on error and set
   errno. */
extern int obj_write(struct repo *repo, struct obj *obj);

/* Free object resources */
extern void obj_close(struct obj *obj);

#endif