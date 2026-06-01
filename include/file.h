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

#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>

#define FILE_PERM (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define DIR_PERM (S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)

/* Check if a file or directory exists. */
extern bool existfile(const char *path);
extern bool existdir(const char *path);

/* Atomically create a directory or file if it does not exist. */
extern int mkdirifne(const char *path, mode_t mode);
extern int mkfileifne(const char *path, mode_t mode);

#endif