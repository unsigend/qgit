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

#ifndef RAWOBJ_RAWOBJ_H
#define RAWOBJ_RAWOBJ_H

#include <libqgit/common.h>
#include <libqgit/types.h>
#include <stddef.h>

typedef struct {
    void *data; /* raw decompressed object data */
    size_t len; /* length of the raw object data */
    qgit_obj_type type;
} qgit_rawobj;

/* Parse the raw decompressed object buf, fill the rawobj fields. Caller should
   free the rawobj buffer. Return 0 on success, -1 on error. */
QGIT_INTERNAL(int)
qgit_rawobj_parse(void *decmpbuf, size_t decmpbuflen, qgit_rawobj *rawobj);

/* Parse the raw decompressed object buf, Parse header only no payload. Return 0
   on success, -1 on error. */
QGIT_INTERNAL(int)
qgit_rawobj_parse_header(void *decmpbuf, size_t decmpbuflen,
                         qgit_rawobj *rawobj);

/* Format the rawobj into a buffer, caller should free the rawobj buffer. Return
   0 on success, -1 on error. */
QGIT_INTERNAL(int)
qgit_rawobj_format(qgit_rawobj *rawobj, void **buf, size_t *buflen);

#endif