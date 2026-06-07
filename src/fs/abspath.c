/* miniutils - A minimal GNU coreutils implementation
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
#include <unistd.h>

int abspath(const char *path, char *buf)
{
  if (realpath(path, buf))
    return 0;
  if (errno != ENOENT)
    return -1;

  if (*path == '/') {
    if (snprintf(buf, PATH_MAX, "%s", path) >= PATH_MAX) {
      errno = ENAMETOOLONG;
      return -1;
    }
    return 0;
  }

  char cwd[PATH_MAX];
  if (getcwd(cwd, PATH_MAX) == NULL)
    return -1;

  if (snprintf(buf, PATH_MAX, "%s/%s", cwd, path) >= PATH_MAX) {
    errno = ENAMETOOLONG;
    return -1;
  }
  return 0;
}