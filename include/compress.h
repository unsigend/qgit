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

#ifndef COMPRESS_H
#define COMPRESS_H

#include <stddef.h>

/* Compress data using zlib. The dest ptr will be a pointer to heap allocated
   memory. Caller should free the memory after use. These functions are just
   wrappers around the zlib functions and set proper errno on error. */

/* Compress data from src to dest. */
extern int zlib_compress(const void *src, size_t srclen, void **dest,
                         size_t *destlen);

/* Compress data from src to a file. */
extern int zlib_compressf(const void *src, size_t srclen, const char *path);

/* Decompress data from src to dest. */
extern int zlib_decompress(const void *src, size_t srclen, void **dest,
                           size_t *destlen);

/* Decompress data from a file to dest. */
extern int zlib_decompressf(const char *path, void **dest, size_t *destlen);

#endif