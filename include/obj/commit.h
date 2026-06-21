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

#ifndef OBJ_COMMIT_H
#define OBJ_COMMIT_H

#include <stdio.h>
#include <time.h>

#include "collection/heap.h"
#include "collection/set.h"
#include "collection/slist.h"
#include "obj/sign.h"
#include "sha1.h"

struct obj;
struct repo;

/* Raw payload format for commit:
     tree <sha1-40>\n
     parent <sha1-40>\n (optional)
     author <name> <email> <timestamp> <timezone>\n
     committer <name> <email> <timestamp> <timezone>\n
     \n
     <message>
*/

struct commit {
  unsigned char tree[SHA1_DIGLEN];
  struct slist parents; /* sha1 strings */
  struct sign author;
  struct sign committer;
  const char *msg;
};

enum commit_print_style {
  COMMIT_PRINT_STYLE_DEFAULT,
  COMMIT_PRINT_STYLE_RAW,
  COMMIT_PRINT_STYLE_ONELINE,
};

extern int commit_parse(struct obj *obj);
extern void commit_close(struct commit *commit);
extern int commit_fprintf(struct obj *obj, FILE *fp);
extern int commit_fprintf_style(struct obj *obj, FILE *fp,
                                enum commit_print_style style);

enum commit_walk_t {
  COMMIT_WALK_FPARENT,
  COMMIT_WALK_ALL,
};

struct commit_iter {
  struct obj *cur;
  struct repo *repo;
  struct heap pq;     /* priority queue with struct obj* */
  struct set visited; /* visited commits with sha1 */
  struct set_fns fns;
  enum commit_walk_t type;
};

/* transfer ownership of object to the iterator */
extern int commit_iter_init(struct commit_iter *iter, struct repo *repo,
                            struct obj *start, enum commit_walk_t type);

/* Return 0 on success, -1 on error, 1 on exhausted */
extern int commit_iter_inc(struct commit_iter *iter);
extern struct obj *commit_iter_get(struct commit_iter *iter);
extern void commit_iter_fini(struct commit_iter *iter);

#endif