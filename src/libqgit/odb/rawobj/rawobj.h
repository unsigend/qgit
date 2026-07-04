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

#ifndef RAWOBJ_H
#define RAWOBJ_H

#include <libqgit/common.h>
#include <libqgit/types.h>
#include <stddef.h>

typedef struct {
    void *data; /* object payload */
    size_t len; /* payload length */
    qgit_obj_type type;
} qgit_rawobj;

/**
 * Parse "<type> <len>\0<payload>". buf may be modified.
 *
 * @param out output raw object, must not be NULL
 * @param buf decompressed buffer, caller frees after success
 * @param buflen length of buf
 * @return 0 on success, -1 on error
 */
QGIT_INTERNAL(int)
qgit_rawobj_parse(qgit_rawobj **out, void *buf, size_t buflen);

/**
 * Parse "<type> <len>\0" header only, set data to NULL.
 *
 * @param out output raw object, must not be NULL
 * @param buf decompressed buffer, caller frees after success
 * @param buflen length of buf
 * @return 0 on success, -1 on error
 */
QGIT_INTERNAL(int)
qgit_rawobj_parse_header(qgit_rawobj **out, void *buf, size_t buflen);

/**
 * Format "<type> <len>\0<payload>" from a raw object.
 *
 * @param obj raw object, must not be NULL
 * @param buf output buffer in heap caller should free it
 * @param buflen output buffer length
 * @return 0 on success, -1 on error
 */
QGIT_INTERNAL(int)
qgit_rawobj_fmt(const qgit_rawobj *obj, void **buf, size_t *buflen);

/**
 * Free a raw object and its owned payload.
 *
 * @param obj raw object, must not be NULL
 */
QGIT_INTERNAL(void)
qgit_rawobj_free(qgit_rawobj *obj);

#endif
