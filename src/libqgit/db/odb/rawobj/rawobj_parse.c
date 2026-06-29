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

#include "rawobj.h"

#include <assert.h>
#include <errno.h>
#include <libqgit/error.h>
#include <libqgit/object/object.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int qgit_rawobj_parse(void *decmpbuf, size_t decmpbuflen, qgit_rawobj *rawobj)
{
    assert(decmpbuf && decmpbuflen && rawobj);

    char *cur = decmpbuf;
    char *end = cur + decmpbuflen;
    char *endstr = NULL;
    qgit_obj_type type = QGIT_OBJ_BAD;

    while (cur < end && *cur != ' ')
        cur++;

    if (cur == end) {
        qgit_seterrno(QGITERR_BADOBJFILE);
        return -1;
    }

    *cur = '\0';

    type = qgit_object_string2type((char *)decmpbuf);
    if (type == QGIT_OBJ_BAD)
        return -1;

    cur++;

    errno = 0;
    size_t size = strtoul((char *)cur, &endstr, 10);
    if (errno || endstr == cur || *endstr) {
        if (!errno)
            qgit_seterrno(QGITERR_BADOBJFILE);
        return -1;
    }
    cur = endstr + 1;
    if (size > (size_t)(end - cur)) {
        qgit_seterrno(QGITERR_BADOBJFILE);
        return -1;
    }

    if (size) {
        rawobj->data = malloc(size);
        if (!rawobj->data)
            return -1;
        memcpy(rawobj->data, cur, size);
    } else
        rawobj->data = NULL;

    rawobj->type = type;
    rawobj->len = size;

    return 0;
}
