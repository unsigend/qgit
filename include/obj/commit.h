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

#ifndef COMMIT_H
#define COMMIT_H

#include <stddef.h>
#include <stdio.h>
#include <time.h>

#include "repo.h"
#include "sha1.h"

struct obj;

struct commit {
  unsigned char tree[SHA1_DIGEST_LENGTH];
  struct slist *parents;
  const char *author;
  const char *committer;
  const char *msg;
  time_t ctime;      /* committer timestamp */
  time_t atime;      /* author timestamp */
  const char *czone; /* committer timezone */
  const char *azone; /* author timezone */
};

typedef enum {
  COMMIT_STYLE_DEFAULT,
  COMMIT_STYLE_ONELINE,
  COMMIT_STYLE_RAW,
} commit_style_t;

/* Raw payload format for commit:
     tree <sha1-40>\n
     parent <sha1-40>\n (optional)
     author <name> <email> <timestamp> <timezone>\n
     committer <name> <email> <timestamp> <timezone>\n
     \n
     <message>
*/
extern int commit_parse(struct obj *obj);
extern void commit_free(struct commit *commit);

/* Pretty print the commit to a stream or buffer. Return 0 on success, -1 on
   error. */
extern int commit_fprintf(FILE *stream, struct obj *obj);

/* Pretty print the commit to a stream or buffer with a specific style. Return
   0 on success, -1 on error. */
extern int commit_fprintf_style(FILE *stream, struct obj *obj,
                                commit_style_t style);

typedef enum {
  COMMIT_WALK_FIRST,
  COMMIT_WALK_ALL,
} commit_walk_type_t;

struct commit_iter {
  struct obj *cur;
  struct repo *repo;
  commit_walk_type_t type;
};

/* Initialize a commit iterator based on the start commit and the repository.
   Return 0 on success, -1 on error and set errno. The obj ownership is
   transferred to the iterator, the caller should not free the obj. And the
   init will parse the start object. */
extern int commit_iter_init(struct commit_iter *iter, struct obj *start,
                            struct repo *repo, commit_walk_type_t type);

/* Increment the iterator to the next commit. Return 0 on success, 1 when there
   is no next commit. -1 on error and set errno.*/
extern int commit_iter_inc(struct commit_iter *iter);

/* Get the current commit in the iterator. Return the current commit on success,
   NULL on error and set errno or exhausted. */
extern struct obj *commit_iter_get(struct commit_iter *iter);

extern void commit_iter_fini(struct commit_iter *iter);

#endif