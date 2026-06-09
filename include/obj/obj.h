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
  OBJ_NONE = -1
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

/* qgit object storage:
  sha1 hash: "<type> <payloadsz>\0<payload>" uncompressed
  file on disk: "<type> <payloadsz>\0<payload>" compressed */

/* Open an object from the repository by its sha1, and fill the sha1 field.
   Return object on success, NULL on error and set errno. */
extern struct obj *obj_open_sha1(struct repo *repo, const unsigned char *sha1);

/* Open an object from a file, return object on success, NULL on error and set
   errno. This function will consider the raw file content as payload, no data
   or type validation is performed. */
extern struct obj *obj_open_file(const char *path, obj_type_t type);

/* Compute the SHA1 of the object, fill the sha1 field with the result.
   Return 0 on success, -1 on error and set errno. */
extern int obj_sha1(struct obj *obj);

/* Write an object to the repository, assume the sha1 is already computed,
   return 0 on success, -1 on error and set errno. */
extern int obj_write(struct repo *repo, struct obj *obj);

/* Free object resources */
extern void obj_close(struct obj *obj);

/* Parse an object based on the type, return 0 on success, -1 on error and set
   errno. */
extern int obj_parse(struct obj *obj);

/* utility functions: have no side effects */

/* Resolve a full hash, short hash or tag name to sha1. Return 0 on success, -1
 * on error and set errno. */
extern int obj_resolve(struct repo *repo, const char *name,
                       unsigned char *sha1);

/* Convert between object type and string. */
extern obj_type_t obj_type_from_str(const char *str);
extern const char *str_from_obj_type(obj_type_t type);

#endif