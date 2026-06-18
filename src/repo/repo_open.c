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

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "fs.h"
#include "repo.h"

struct repo *repo_open(const char *path)
{
  if (!path)
    return NULL;

  char buf[PATH_MAX];
  struct repo *repo;

  if (!dir_exists(path)) {
    if (!errno)
      errno = ENOTDIR;
    return NULL;
  }

  if (snprintf(buf, PATH_MAX, "%s/.qgit", path) >= PATH_MAX) {
    errno = ENAMETOOLONG;
    return NULL;
  }

  if (!dir_exists(buf)) {
    setqerrno(QE_NOTINREPO);
    return NULL;
  }

  if (!(repo = calloc(1, sizeof(struct repo))))
    return NULL;

  if (!(repo->worktree = strdup(path))) {
    repo_close(repo);
    return NULL;
  }

  if (!(repo->qgitdir = strdup(buf))) {
    repo_close(repo);
    return NULL;
  }
  return repo;
}