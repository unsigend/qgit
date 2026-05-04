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

#include "object.h"

struct commit {
  char tree[41];
  char **parents;
  int nparents;
  char *author;
  char *committer;
  char *message;
};

/* Parse a commit object from the payload */
extern struct commit *commit_parse(struct object *obj);

/* Release the resources of the commit. */
extern void commit_free(struct commit *commit);

#endif