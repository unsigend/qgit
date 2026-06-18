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

#include "collection/slist.h"
#include "sha1.h"

struct obj;

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
  struct slist parents; /* sha1 strings*/
  const char *author;
  const char *committer;
  const char *msg;
  time_t atime;      /* author time */
  time_t ctime;      /* committer time */
  const char *azone; /* author timezone */
  const char *czone; /* committer timezone */
};

extern int commit_parse(struct obj *obj);
extern void commit_close(struct commit *commit);
extern int commit_fprintf(struct obj *obj, FILE *fp);

#endif