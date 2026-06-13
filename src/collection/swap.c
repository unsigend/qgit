/* collection - A generic data structure and algorithms library
 * Copyright (C) 2025 Yixiang Qiu
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

#include <stdint.h>
#include <string.h>

#include "collection/util.h"

void swap(void *l, void *r, size_t sz)
{
  uint64_t u64;
  size_t i = 0;
  for (; i + 8 <= sz; i += 8) {
    memcpy(&u64, l + i, 8);
    memcpy(l + i, r + i, 8);
    memcpy(r + i, &u64, 8);
  }
  uint8_t u8;
  for (; i < sz; i++) {
    memcpy(&u8, l + i, 1);
    memcpy(l + i, r + i, 1);
    memcpy(r + i, &u8, 1);
  }
}