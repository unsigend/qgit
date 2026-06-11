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
#include <stdlib.h>
#include <zlib.h>

int zlib_compress(const void *src, size_t srclen, void **dest, size_t *destlen)
{
  uLongf buflen = compressBound(srclen);
  void *buf = malloc(buflen);
  if (!buf)
    return -1;

  int ret = compress2(buf, &buflen, src, srclen, Z_BEST_COMPRESSION);
  if (ret != Z_OK) {
    if (ret == Z_MEM_ERROR)
      errno = ENOMEM;
    else if (ret == Z_BUF_ERROR)
      errno = EINVAL;
    else
      errno = EIO;
    free(buf);
    return -1;
  }

  void *newbuf = realloc(buf, buflen); /* shrink to fit */
  if (!newbuf) {
    free(buf);
    return -1;
  }
  buf = newbuf;

  *dest = buf;
  *destlen = buflen;
  return 0;
}

int zlib_decompress(const void *src, size_t srclen, void **dest,
                    size_t *destlen)
{
  uLongf buflen = srclen * 4;
  void *buf = malloc(buflen);
  if (!buf)
    return -1;

  int ret;
  while ((ret = uncompress(buf, &buflen, src, srclen)) == Z_BUF_ERROR) {
    buflen *= 2;
    void *newbuf = realloc(buf, buflen);
    if (!newbuf) {
      free(buf);
      return -1;
    }
    buf = newbuf;
  }

  if (ret != Z_OK) {
    if (ret == Z_MEM_ERROR)
      errno = ENOMEM;
    else
      errno = EIO;
    free(buf);
    return -1;
  }

  void *newbuf = realloc(buf, buflen); /* shrink to fit */
  if (!newbuf) {
    free(buf);
    return -1;
  }
  buf = newbuf;

  *dest = buf;
  *destlen = buflen;
  return 0;
}