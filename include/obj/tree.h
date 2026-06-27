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

#ifndef OBJ_TREE_H
#define OBJ_TREE_H

#include <stdio.h>
#include <sys/stat.h>

#include "collection/vector.h"
#include "sha1.h"

struct object;
struct repo;

/* Raw payload format for tree:
     <mode> <path>\0<sha1-20>
     ...
     <mode> <path>\0<sha1-20>
*/

struct tree_entry {
  mode_t mode;
  const char *path;
  unsigned char sha1[SHA1_DIGLEN];
};

struct tree {
  struct vector entries;
};

enum tree_print_style {
  TREE_PRINT_STYLE_DEFAULT,
  TREE_PRINT_STYLE_RECURSE,
  TREE_PRINT_STYLE_SHOW_TREE,
};

typedef int (*tree_foreach_cb)(struct tree_entry *entry, const char *prefix,
                               void *arg);

extern int tree_parse(struct object *obj);
extern struct object *tree_create(const struct tree *tree);
extern void tree_close(struct tree *tree);
extern int tree_fprintf(struct object *obj, FILE *fp);
extern int tree_fprintf_style(struct object *obj, FILE *fp, struct repo *repo,
                              enum tree_print_style style);
extern int tree_foreach(struct object *obj, struct repo *repo,
                        tree_foreach_cb cb, void *arg);

/* internal functions */
extern int tree_entry_fprintf(struct tree_entry *entry, const char *prefix,
                              FILE *fp);

/* tree sort comparator */
extern int tree_entry_cmp(const void *a, const void *b);

#endif