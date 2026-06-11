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

#ifndef TREE_H
#define TREE_H

#include <stddef.h>
#include <stdio.h>

#include "collection/vector.h"
#include "sha1.h"

struct obj;

struct tree_entry {
  const char *mode;
  const char *path;
  unsigned char sha1[SHA1_DIGEST_LENGTH];
};

struct tree {
  struct vector entries;
};

/* Raw payload format for tree:
     <mode> <path>\0<sha1-20>
     ...
     <mode> <path>\0<sha1-20>
*/
extern int tree_parse(struct obj *obj);
extern void tree_free(struct tree *tree);

/* Pretty print the tree to a stream or buffer. Return the number of bytes
   written on success, -1 on error. */
extern int tree_fprintf(FILE *stream, struct obj *obj);

#endif