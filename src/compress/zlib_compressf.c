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
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <zlib.h>

#include "compress.h"
#include "fs.h"

int zlib_compressf(const void *src, size_t srclen, const char *path)
{
  mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
  int fd = -1;
  void *dest = NULL;
  size_t destlen = 0;

  if (zlib_compress(src, srclen, &dest, &destlen) == -1)
    return -1;

  if ((fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, mode)) == -1) {
    free(dest);
    return -1;
  }

  if (write_all(fd, dest, destlen) == -1) {
    free(dest);
    close(fd);
    unlink(path);
    return -1;
  }
  free(dest);
  close(fd);
  return 0;
}