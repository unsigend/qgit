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
#include <stdio.h>
#include <string.h>

#include "fs.h"
#include "repo.h"

struct repo *repo_find(const char *path)
{
  if (!path)
    return NULL;

  char buf[PATH_MAX];
  char qgit[PATH_MAX];

  if (snprintf(buf, sizeof(buf), "%s", path) >= PATH_MAX) {
    errno = ENAMETOOLONG;
    return NULL;
  }

  while (1) {
    if (snprintf(qgit, sizeof(qgit), "%s/.qgit", buf) >= PATH_MAX) {
      errno = ENAMETOOLONG;
      return NULL;
    }
    if (dir_exists(qgit))
      break;
    char *slash = strrchr(buf, '/');
    *slash = '\0';
    if (slash == buf) {
      errno = ENOENT;
      return NULL;
    }
  }

  struct repo *repo = repo_init(buf);
  if (!repo)
    return NULL;

  return repo;
}