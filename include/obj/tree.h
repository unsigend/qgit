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
#include <sys/stat.h>

#include "collection/vector.h"
#include "repo.h"
#include "sha1.h"

struct obj;

struct tree_entry {
  mode_t mode;
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

/* Pretty print a tree entry to a stream or buffer. Return 0 on success, -1 on
   error. */
extern int tree_entry_fprintf(FILE *stream, struct tree_entry *entry,
                              const char *prefix);

/* Pretty print the tree to a stream or buffer. Return 0 on success, -1 on
   error. */
extern int tree_fprintf(FILE *stream, struct obj *obj);

/* Pretty print the tree to a stream or buffer recursively. Return 0 on success,
   -1 on error. if showtree is false, show blob only. */
extern int tree_fprintf_r(FILE *stream, struct obj *obj, struct repo *repo,
                          int showtree);

/* Callback function for tree traversal. Return 0 on success, -1 on error and
   set errno. */
typedef int (*tree_traverse_cb)(struct tree_entry *entry, const char *prefix,
                                void *arg);

/* Tree Traversal: Traverse the tree and call the callback for each entry.
   Return 0 on success, -1 on error and set errno. */
extern int tree_traverse(struct obj *obj, tree_traverse_cb cb,
                         struct repo *repo, void *arg);

#endif