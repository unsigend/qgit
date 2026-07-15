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

#include <assert.h>
#include <sha1.h>
#include <stdlib.h>

int qgit_odb_hash(qgit_oid *oid, const void *data, size_t len,
                  qgit_obj_type type)
{
    assert(oid);

    qgit_rawobj rawobj;
    unsigned char sha[QGIT_OID_RAWSZ];
    void *buf;
    size_t buflen;

    rawobj.data = (void *)data;
    rawobj.len = len;
    rawobj.type = type;

    if (qgit_rawobj_fmt(&rawobj, &buf, &buflen))
        return -1;

    if (sha1(buf, buflen, sha)) {
        free(buf);
        return -1;
    }

    qgit_oid_fromraw(oid, sha);
    free(buf);

    return 0;
}
