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

#include "iniparse.h"
#include <stdbool.h>
#include <stddef.h>

struct repo {
  char *worktree;
  char *qgit;
  bool bare;
  bool reinit;
  struct iniFILE *config;
};

extern struct repo *repo_open(const char *abspath);
extern struct repo *repo_create(const char *abspath, const char *bname,
                                bool bare);
extern void repo_close(struct repo *repo);

#endif