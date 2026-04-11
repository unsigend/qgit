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

/* SHA1 hash function, a thin wrapper around the openssl sha1 function, write
   the 20 bytes raw hash to the buffer. */
extern unsigned char *sha1_hash(const unsigned char *data, size_t len,
                                unsigned char *buf);

/* Convert the 20 bytes raw hash to a 41 bytes hex string, the buffer is
   guaranteed to be null-terminated. */
extern unsigned char *sha1_hex(const unsigned char hash[20],
                               unsigned char buf[41]);

#endif