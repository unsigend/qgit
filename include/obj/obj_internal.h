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

#ifndef OBJ_OBJ_INTERNAL_H
#define OBJ_OBJ_INTERNAL_H

#include <stddef.h>

struct object;
struct repo;

/* internal plumbing functions, these functions handle raw bytes buffer
   "type <size>\0<payload>" */

extern int obj_write_buf(struct object *obj, void **buf, size_t *buflen);
extern int obj_store(struct repo *repo, const unsigned char *sha1,
                     const void *buf, size_t buflen);

#endif