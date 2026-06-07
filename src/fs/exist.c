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
  if (stat(path, &st) == -1)
    return 0;
  return S_ISREG(st.st_mode);
}

int dir_exists(const char *path)
{
  struct stat st;
  if (stat(path, &st) == -1)
    return 0;
  return S_ISDIR(st.st_mode);
}