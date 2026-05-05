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

#include <openssl/sha.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

unsigned char *sha1_hash(const unsigned char *data, size_t len,
                         unsigned char *buf)
{
  return SHA1(data, len, buf);
}

unsigned char *sha1_to_hex(const unsigned char hash[20], unsigned char buf[41])
{
  static const char hex[] = "0123456789abcdef";
  for (size_t i = 0; i < 20; i++) {
    unsigned char hi = hash[i] >> 4;
    unsigned char lo = hash[i] & 0x0f;
    buf[i * 2] = hex[hi];
    buf[i * 2 + 1] = hex[lo];
  }
  buf[40] = '\0';
  return buf;
}

static unsigned hex_val(unsigned char c)
{
  if (c >= '0' && c <= '9')
    return (unsigned)(c - '0');
  if (c >= 'a' && c <= 'f')
    return (unsigned)(c - 'a' + 10);
  if (c >= 'A' && c <= 'F')
    return (unsigned)(c - 'A' + 10);
  return 0;
}

unsigned char *hex_to_sha1(const unsigned char hex[41], unsigned char sha1[20])
{
  for (size_t i = 0; i < 20; i++) {
    unsigned char hi = hex[i * 2];
    unsigned char lo = hex[i * 2 + 1];
    sha1[i] = (hex_val(hi) << 4) | hex_val(lo);
  }
  return sha1;
}