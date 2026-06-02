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
#include <limits.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int path_exists(const char *path)
{
  struct stat st;
  return stat(path, &st) == 0;
}

int file_exists(const char *path)
{
  struct stat st;
  return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

int dir_exists(const char *path)
{
  struct stat st;
  return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

int mkfile_safe(const char *path, mode_t mode)
{
  int fd = open(path, O_WRONLY | O_CREAT | O_EXCL, mode);
  if (fd == -1) {
    if (errno == EEXIST) {
      struct stat st;
      if (stat(path, &st) == 0 && S_ISREG(st.st_mode))
        return 0;
      return -1;
    }
    return -1;
  }
  close(fd);
  return 0;
}

int mkdir_safe(const char *path, mode_t mode)
{
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

int mkdirp(const char *path, mode_t mode)
{
  if (strlen(path) >= PATH_MAX)
    return -1;
  if (mkdir(path, mode) == 0)
    return 0;
  if (errno == EEXIST) {
    struct stat st;
    if (stat(path, &st) == 0 && S_ISDIR(st.st_mode))
      return 0;
    return -1;
  }
  if (errno != ENOENT)
    return -1;

  char parent[PATH_MAX];
  strncpy(parent, path, PATH_MAX - 1);
  parent[PATH_MAX - 1] = '\0'; /* avoid buffer overflow */
  char *slash = strrchr(parent, '/');
  if (!slash || slash == parent)
    return -1;

  *slash = '\0';

  if (mkdirp(parent, mode) == -1)
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