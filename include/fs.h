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

#ifndef FS_H
#define FS_H

#include <stddef.h>
#include <sys/types.h>

/* Read or Write all data from/to a file descriptor. Retry on EINTR or EAGAIN.
   Return the number of bytes read or written on success, -1 on error and set
   errno. */
extern ssize_t read_all(int fd, void *buf, size_t n);
extern ssize_t write_all(int fd, const void *buf, size_t n);

/* Copy file from src to dst, create dst file if it does not exist, Return 0 on
   success, -1 on error and set errno. */
extern int copy_file(const char *src, const char *dst);

/* Copy one directory's content to another directory, both src and dst must
   exists. Return 0 on success, -1 on error and set errno. */
extern int copy_dir(const char *src, const char *dst);

/* Check if path exists, Return 1 if it exists, 0 if it does not exist. */
extern int path_exists(const char *path);

/* Check if a path is a file or directory, Return 1 if file/dir exists, 0 if it
   does not exist. */
extern int file_exists(const char *path);
extern int dir_exists(const char *path);

/* Create a directory and all parent directories if they do not exist.
   Return 0 on success, -1 on error and set errno. */
extern int mkdirp(const char *path, mode_t mode);

/* Remove a directory and all its contents. Recursively remove all files and
   directories. Return 0 on success, -1 on error and set errno. */
extern int rmdirr(const char *path);

/* Resolve the absolute path of a given path. Wrap on top of realpath(), but
   accept non-existing paths. Assume the buf is large enough to hold the
   PATH_MAX. Return 0 on success, -1 on error and set errno. */
extern int fabspath(const char *path, char *buf);

/* Get the basename of a given path. Return the basename on success, NULL on
   error and set errno. Compatible with POSIX and BSD. */
extern char *fbasename(const char *path, char *buf);

#endif