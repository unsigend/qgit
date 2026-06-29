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

#include "loose_backend.h"

#include <assert.h>
#include <compress.h>
#include <errno.h>
#include <fs.h>
#include <libqgit/db/oid.h>
#include <libqgit/error.h>
#include <libqgit/object/object.h>
#include <libqgit/types.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int loose_backend_read(void **data_p, size_t *len_p, qgit_obj_type *type_p,
                       struct qgit_odb_backend *backend, const qgit_oid *oid)
{
    assert(data_p && len_p && type_p && backend && oid);
    *data_p = NULL;
    *len_p = 0;

    struct loose_backend *loose_backend = (struct loose_backend *)backend;
    char oidpath[QGIT_OID_HEXSZ + 2];
    char path[PATH_MAX];

    void *decmpbuf, *payload;
    size_t decmpbuflen = 0, payloadlen = 0;

    if (qgit_oid_fmtpath(oidpath, oid) < 0)
        return -1;

    if (snprintf(path, PATH_MAX, "%s/%s", loose_backend->objects_dir,
                 oidpath) >= PATH_MAX) {
        errno = ENAMETOOLONG;
        return -1;
    }

    if (zlib_decompressf(path, &decmpbuf, &decmpbuflen) == -1)
        return -1;

    if (loose_parse_raw(decmpbuf, decmpbuflen, type_p, &payload, &payloadlen) ==
        -1) {
        free(decmpbuf);
        return -1;
    }

    if (payloadlen > 0) {
        *data_p = malloc(payloadlen);
        if (!*data_p) {
            free(decmpbuf);
            return -1;
        }

        memcpy(*data_p, payload, payloadlen);
    } else
        *data_p = NULL;

    *len_p = payloadlen;

    free(decmpbuf);
    return 0;
}