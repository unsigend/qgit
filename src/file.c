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
#include <fcntl.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <unistd.h>

bool existfile(const char *path)
{
  if (!path || !*path)
    return false;
  struct stat st;
  if (stat(path, &st) == -1)
    return false;
  return S_ISREG(st.st_mode);
}

bool existdir(const char *path)
{
  if (!path || !*path)
    return false;
  struct stat st;
  if (stat(path, &st) == -1)
    return false;
  return S_ISDIR(st.st_mode);
}

int mkdirifne(const char *path, mode_t mode)
{
  if (!path || !*path)
    return -1;

  if (mkdir(path, mode) == 0)
    return 0;

  if (errno == EEXIST) {
    struct stat st;
    if (stat(path, &st) == 0 && S_ISDIR(st.st_mode))
      return 0;
    return -1;
  }
  return -1;
}

int mkfileifne(const char *path, mode_t mode)
{
  if (!path || !*path)
    return -1;

  /* Atomic creation */
  int fd = open(path, O_CREAT | O_WRONLY | O_EXCL, mode);
  if (fd != -1) {
    close(fd);
    return 0;
  }
  if (errno == EEXIST) {
    struct stat st;
    if (stat(path, &st) == 0 && S_ISREG(st.st_mode))
      return 0;
  }
  return -1;
}