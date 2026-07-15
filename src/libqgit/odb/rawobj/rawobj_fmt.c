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
#include <libqgit/object/object.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int qgit_rawobj_fmt(const qgit_rawobj *obj, void **buf, size_t *buflen)
{
    assert(obj && buf && buflen);

    if (obj->len && !obj->data)
        return -1;

    const char *type_str = qgit_object_type2string(obj->type);
    int n = 0;
    size_t len = 0;

    if (!type_str)
        return -1;

    if ((n = snprintf(NULL, 0, "%s %zu", type_str, obj->len)) < 0)
        return -1;

    len = n + obj->len + 1;
    if (!(*buf = malloc(len)))
        return -1;

    char *cur = *buf;
    if ((n = snprintf(cur, len, "%s %zu", type_str, obj->len)) < 0) {
        free(*buf);
        *buf = NULL;
        return -1;
    }

    cur += n;
    *cur++ = '\0';

    if (obj->data && obj->len)
        memcpy(cur, obj->data, obj->len);

    *buflen = len;
    return 0;
}
