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
#include <stdio.h>

unsigned char *sha1_hash(const unsigned char *data, size_t len,
                         unsigned char *buf)
{
  return SHA1(data, len, buf);
}

unsigned char *sha1_hex(const unsigned char hash[20], unsigned char buf[41])
{
  for (size_t i = 0; i < 20; i++)
    snprintf((char *)(buf + i * 2), 3, "%02x", hash[i]);
  buf[40] = '\0';
  return buf;
}