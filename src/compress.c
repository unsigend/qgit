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

#include <stdlib.h>
#include <zlib.h>

int zlib_compress(const unsigned char *src, size_t srclen, unsigned char **dest,
                  size_t *destlen)
{
  *destlen = compressBound(srclen);
  *dest = malloc(*destlen);
  if (!*dest)
    return -1;
  int ret = compress2(*dest, destlen, src, srclen, Z_BEST_COMPRESSION);
  if (ret != Z_OK) {
    free(*dest);
    *dest = NULL;
    return -1;
  }
  *dest = realloc(*dest, *destlen);
  if (!*dest) {
    free(*dest);
    *dest = NULL;
    return -1;
  }
  return 0;
}

int zlib_decompress(const unsigned char *src, size_t srclen,
                    unsigned char **dest, size_t *destlen)
{
  size_t buflen = srclen * 4;
  unsigned char *buf = malloc(buflen);
  if (!buf)
    return -1;
  int ret;
  while ((ret = uncompress(buf, &buflen, src, srclen)) == Z_BUF_ERROR) {
    buflen *= 2;
    unsigned char *newbuf = realloc(buf, buflen);
    if (!newbuf) {
      free(buf);
      *dest = NULL;
      return -1;
    }
    buf = newbuf;
  }
  if (ret != Z_OK) {
    free(buf);
    *dest = NULL;
    return -1;
  }
  *dest = buf;
  *destlen = buflen;
  return 0;
}