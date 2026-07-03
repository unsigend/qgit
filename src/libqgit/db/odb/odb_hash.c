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

#include "odb.h"
#include "rawobj/rawobj.h"

#include <assert.h>
#include <libqgit/db/oid.h>
#include <sha1.h>
#include <stdlib.h>

int qgit_odb_hash(qgit_oid *out, const void *data, size_t len,
                  qgit_obj_type type)
{
    assert(out && type);

    unsigned char sha[QGIT_OID_RAWSZ];
    qgit_rawobj rawobj = {
        .data = (void *)data,
        .len = len,
        .type = type,
    };

    void *buf = NULL;
    size_t buflen = 0;

    if (qgit_rawobj_format(&rawobj, &buf, &buflen) == -1)
        return -1;
    if (sha1(buf, buflen, sha) == -1) {
        free(buf);
        return -1;
    }

    qgit_oid_fromraw(out, sha);
    free(buf);
    return 0;
}