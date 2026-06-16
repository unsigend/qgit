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

#ifndef REF_H
#define REF_H

/* qgit reference layout:
   .qgit/HEAD,
   .qgit/refs/heads/<branch>,
   .qgit/refs/tags/<tag>
   for remote reference not supported yet
*/

#include "repo.h"

/* Read a ref from the repository, write the sha1 to the sha1 buffer. Return 0
   on success, -1 on error and set errno. Refname must the full path of the ref
   like refs/heads/main or HEAD. */
extern int ref_read(struct repo *repo, const char *refname,
                    unsigned char *sha1);

typedef int (*ref_cb)(const char *refname, const unsigned char *sha1,
                      void *arg);

/* Traverse all refs in the repository with a given prefix. */
extern int ref_foreach(struct repo *repo, const char *prefix, ref_cb cb,
                       void *arg);

#endif