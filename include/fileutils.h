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

#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <sys/types.h>

/* Read or Write all data from/to a file descriptor. Retry on EINTR or EAGAIN.
   Return the number of bytes read or written on success, -1 on error and set
   errno. */
extern ssize_t read_all(int fd, void *buf, size_t n);
extern ssize_t write_all(int fd, const void *buf, size_t n);

/* Read a file content into a buffer, the buf needs free by the caller. The buf
   is guaranteed to be '\0' terminated if the file is not empty. Return 0 on
   success, -1 on error and set errno. */
extern int read_file(const char *path, void **buf, size_t *len);

/* Write a buffer to a file if not empty truncate the file. Create atomically if
   not exists. Return 0 on success, -1 on error and set errno. */
extern int write_file(const char *path, const void *buf, size_t buflen);

/* Copy file from src to dest, create dest file if it does not exist. Return 0
   on success, -1 on error and set errno. */
extern int copy_file(const char *dest, const char *src);

/* Copy one directory's content to another directory, both dest and src must
   exist. Return 0 on success, -1 on error and set errno. */
extern int copy_dir(const char *dest, const char *src);

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