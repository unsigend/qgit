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

#include <limits.h>

struct repo {
  char worktree[PATH_MAX];
  char gitdir[PATH_MAX];
};

/* Initialize a repository at the given path. Return the repository on success,
   NULL on error and set errno. For the repository format only support .qgit dir
   under the worktree. */
extern struct repo *repo_init(const char *path);

extern void repo_free(struct repo *repo);

/* Create a repository or re-initialize an existing one. Return 0 on success,
   -1 on error and set errno. */
extern int repo_create(struct repo *repo, const char *branch);

/* From a given path, find the repository and return the repository on success,
   NULL if not found. */
extern struct repo *repo_find(const char *path);

/* Return the repository of the current working directory. And recursively up to
   the root. NULL if not found. */
extern struct repo *repo_cwd(void);

#endif