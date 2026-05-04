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
#include <stdbool.h>
#include <stddef.h>

struct repo {
  char *worktree;
  char *qgit;
  bool bare;
  bool reinit;
};

/* Open an existing repository with the absolute path of the worktree. If the
   repository is not found, return NULL. */
extern struct repo *repo_open(const char *abspath);

/* Create a new repository with the absolute path of the worktree, the name of
   the initial branch and whether the repository is bare mode. */
extern struct repo *repo_create(const char *abspath, const char *bname,
                                bool bare);

/* Release the resources of the repository. */
extern void repo_close(struct repo *repo);

/* Find the repository in the path or its ancestors, path is relative path e.g.
   ".", NULL if not found. */
extern struct repo *repo_find(const char *path);

/* Get the path of the object in the repository, the hash is a 41 bytes hex
   string. */
extern char *repo_obj_path(struct repo *repo, const char *hash,
                           char buf[PATH_MAX]);

#endif