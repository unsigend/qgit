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
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "repo.h"

struct repo *repo_find(const char *path)
{
  if (!path)
    return NULL;

  struct repo *repo;
  char buf[PATH_MAX];
  char *slash;

  if ((repo = repo_open(path)))
    return repo;

  if (strcmp(path, "/") == 0) {
    setqerrno(QE_NOTINREPO);
    return NULL;
  }

  strncpy(buf, path, PATH_MAX - 1);
  buf[PATH_MAX - 1] = '\0';
  slash = strrchr(buf, '/');
  if (slash == buf) {
    setqerrno(QE_NOTINREPO);
    return NULL;
  }

  *slash = '\0';

  return repo_find(buf);
}