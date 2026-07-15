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
#include <stdlib.h>

int qgit_rawobj_parse_header(qgit_rawobj **out, void *buf, size_t buflen)
{
    assert(buf && buflen && out);
    *out = NULL;

    char *cur = buf;
    char *end = cur + buflen;
    char *endstr = NULL;
    qgit_obj_type type = QGIT_OBJ_BAD;

    while (cur < end && *cur != ' ')
        cur++;

    if (cur == end) {
        qgit_seterror(QGITERR_BADOBJFILE);
        return -1;
    }

    *cur = '\0';

    type = qgit_object_string2type((char *)buf);
    if (type == QGIT_OBJ_BAD)
        return -1;

    cur++;

    errno = 0;
    size_t size = strtoul((char *)cur, &endstr, 10);
    if (errno || endstr == cur || endstr >= end || *endstr) {
        if (!errno)
            qgit_seterror(QGITERR_BADOBJFILE);
        return -1;
    }
    cur = endstr + 1;

    *out = calloc(1, sizeof(qgit_rawobj));
    if (!*out)
        return -1;

    (*out)->type = type;
    (*out)->len = size;
    (*out)->data = NULL;

    return 0;
}