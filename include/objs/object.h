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

#ifndef OBJECT_H
#define OBJECT_H

#include "repo.h"
#include <stddef.h>

#define OBJ_COMMIT 1
#define OBJ_TREE 2
#define OBJ_BLOB 3
#define OBJ_TAG 4

struct object {
  int type;
  char sha1[41];
  unsigned char *payload;
  size_t size; /* the size of the payload */
};

extern struct object *obj_open(int type, const char *filename);
extern int obj_write(struct repo *repo, struct object *obj);
extern struct object *obj_read(struct repo *repo, const char *sha1);
extern void obj_free(struct object *obj);

/* Compute the hash of the object: <type> <size>\0<payload> */
extern int obj_hash(struct object *obj);

#endif