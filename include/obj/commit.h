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

#include "collection/slist.h"
#include "repo.h"
#include "sha1.h"

struct obj;

struct commit {
  unsigned char tree[SHA1_DIGEST_LENGTH];
  struct slist *parents;
  const char *author;
  const char *committer;
  const char *message;
};

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

typedef enum {
  COMMITWK_FIRST,
  COMMITWK_ALL,
} commit_walk_type_t;

/* Callback function, return -1 for errno, 0 for continue, 1 for stop */
typedef int (*commit_walk_cb)(struct obj *obj, void *arg);

/* Walk through the commit history from the given commit, exit when callback
   return -1 or 1, return 0 on success, -1 on error and set errno. */
extern int commit_walk(struct obj *obj, commit_walk_type_t type,
                       struct repo *repo, commit_walk_cb cb, void *arg);

#endif