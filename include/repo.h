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

#include <stdbool.h>
#include <stddef.h>

struct repo {
  char *worktree;
  char *qgit_dir;
  bool bare;
};

/* Initialize a repository. Check whether exists if check is set. */
extern int repo_init(struct repo *repo, const char *path, bool bare,
                     bool check);
extern int repo_create(const struct repo *repo, const char *bname);
extern void repo_fini(struct repo *repo);

#endif