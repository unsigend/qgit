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

#ifndef SHA1_H
#define SHA1_H

#include <stddef.h>

#define SHA1_DIGLEN 20
#define SHA1_HEXLEN (SHA1_DIGLEN * 2 + 1)

/* Compute the SHA1 digest of the given data. */
extern int sha1(const void *data, size_t len, unsigned char sha1[SHA1_DIGLEN]);

#endif