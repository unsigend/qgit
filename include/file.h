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

#ifndef FILE_H
#define FILE_H

#include <sys/stat.h>

/* File system extensions, path must be absolute path, UB if not */

/* Check if a path exists, 0 if not exists, 1 if exists */
extern int path_exists(const char *path);
extern int file_exists(const char *path);
extern int dir_exists(const char *path);

/* Atomically create a file or dir if not exists, 0 on success, -1 on error */
extern int mkfile_safe(const char *path, mode_t mode);
extern int mkdir_safe(const char *path, mode_t mode);

/* Create a directory and its parents if not exists, 0 on success, -1 on error */
extern int mkdirp(const char *path, mode_t mode);

#endif