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

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
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

int rmrf(const char *path)
{
  struct stat st;
  if (stat(path, &st) == -1)
    return 0; /* Not exists */

  if (S_ISDIR(st.st_mode)) {
    if (rmdir(path) == 0 || errno == ENOENT)
      return 0;
    if (errno != ENOTEMPTY)
      return -1;

    DIR *dir = opendir(path);
    if (!dir)
      return -1;

    struct dirent *entry;
    char child[PATH_MAX];
    int ret = 0;

    while ((entry = readdir(dir)) != NULL) {
      if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        continue;
      snprintf(child, sizeof(child), "%s/%s", path, entry->d_name);
      if (rmrf(child) == -1) {
        ret = -1;
        break;
      }
    }
    closedir(dir);
    return ret == -1 ? -1 : rmdir(path);
  } else {
    return remove(path);
  }
}

int abspath(const char *path, char *abspath)
{
  if (!path || !*path || !abspath || strlen(path) >= PATH_MAX)
    return -1;

  if (realpath(path, abspath))
    return 0;

  if (path[0] == '/') {
    strcpy(abspath, path);
    return 0;
  }

  char cwd[PATH_MAX];
  if (getcwd(cwd, PATH_MAX) == NULL)
    return -1;

  if (snprintf(abspath, PATH_MAX, "%s/%s", cwd, path) >= PATH_MAX)
    return -1;
  return 0;
}