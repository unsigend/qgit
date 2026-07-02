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

#ifndef COLLECTION_UTIL_H
#define COLLECTION_UTIL_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Exchange the contents of two memory regions of sz bytes each.
 *
 * When sz is zero, neither region is read or written. l and r must each point
 * to at least sz bytes and must not overlap.
 *
 * @param l  pointer to the first object
 * @param r  pointer to the second object
 * @param sz number of bytes to exchange, usually sizeof the value type
 */
extern void swap(void *l, void *r, size_t sz);

#ifdef __cplusplus
}
#endif

#endif
