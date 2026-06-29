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

#include "../../odb/odb.h"
#include "loose_backend.h"

#include <assert.h>
#include <compress.h>
#include <errno.h>
#include <fs.h>
#include <libqgit/error.h>
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
    char path[PATH_MAX];

    void *decmpbuf;
    size_t decmpbuflen;
    qgit_rawobj rawobj = {.data = NULL, .len = 0, .type = QGIT_OBJ_BAD};

    if (loose_oid_path(loose_backend->objects_dir, oid, path, PATH_MAX) == -1)
        return -1;

    if (zlib_decompressf(path, &decmpbuf, &decmpbuflen) == -1) {
        if (errno == ENOENT)
            qgit_seterrno(QGITERR_OBJ_NOT_FOUND);
        else
            qgit_clearerrno();
        return -1;
    }

    if (qgit_rawobj_parse(decmpbuf, decmpbuflen, &rawobj) == -1) {
        free(decmpbuf);
        return -1;
    }

    *data_p = rawobj.data; /* transfer ownership to caller */
    *len_p = rawobj.len;
    *type_p = rawobj.type;

    free(decmpbuf);
    return 0;
}