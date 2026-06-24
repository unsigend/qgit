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

#ifndef REPO_H
#define REPO_H

#include <stddef.h>
#include <stdio.h>

struct repo {
  const char *worktree;
  const char *qgitdir;
};

extern struct repo *repo_open(const char *path);
extern struct repo *repo_find(const char *path);
extern struct repo *repo_findcwd(void); /* find repo start from "." */
extern struct repo *repo_create(const char *path, const char *branch);

extern FILE *repo_open_obj(struct repo *repo, unsigned char *sha1);

extern void repo_close(struct repo *repo);

#endif