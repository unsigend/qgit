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

/* Compress data from src to dest. Return 0 on success, -1 on error. */
extern int zlib_compress(const void *src, size_t srclen, void **dest,
                         size_t *destlen);

/* Decompress data from src to dest. Return 0 on success, -1 on error. */
extern int zlib_decompress(const void *src, size_t srclen, void **dest,
                           size_t *destlen);

#endif