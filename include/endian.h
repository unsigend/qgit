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

#ifndef QGIT_ENDIAN_H
#define QGIT_ENDIAN_H

#include <arpa/inet.h>
#include <stdint.h>
#include <string.h>

__attribute__((always_inline)) static inline uint16_t
get_be16(const unsigned char *p)
{
  uint16_t x;
  memcpy(&x, p, sizeof(uint16_t));
  return ntohs(x);
}

__attribute__((always_inline)) static inline uint32_t
get_be32(const unsigned char *p)
{
  uint32_t x;
  memcpy(&x, p, sizeof(uint32_t));
  return ntohl(x);
}

__attribute__((always_inline)) static inline void put_be16(unsigned char *p,
                                                           uint16_t x)
{
  x = htons(x);
  memcpy(p, &x, sizeof(uint16_t));
}

__attribute__((always_inline)) static inline void put_be32(unsigned char *p,
                                                           uint32_t x)
{
  x = htonl(x);
  memcpy(p, &x, sizeof(uint32_t));
}

#endif