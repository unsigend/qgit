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

#ifndef BLOB_H
#define BLOB_H

#include <stdio.h>

struct obj;

struct blob {
};

/* Blob parse no-op, return 0 on success. */
extern int blob_parse(struct obj *obj);
extern void blob_free(struct blob *blob);

/* Pretty print the blob to a stream or buffer. Return 0 on success, -1 on
   error. */
extern int blob_fprintf(FILE *stream, struct obj *obj);

#endif