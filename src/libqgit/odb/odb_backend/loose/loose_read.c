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

#include "../../odb_internal.h"
#include "libqgit/oid.h"
#include "loose_backend.h"

#include <assert.h>
#include <compress.h>
#include <errno.h>
#include <libqgit/db/odb_backend.h>
#include <libqgit/error.h>
#include <limits.h>
#include <stdlib.h>

int qgit_loose_backend_read(void **data_p, size_t *len_p, qgit_obj_type *type_p,
                            struct qgit_odb_backend *backend,
                            const qgit_oid *oid)
{
    assert(data_p && len_p && type_p && backend && oid);

    char path[PATH_MAX];
    struct qgit_loose_backend *loose_backend =
        (struct qgit_loose_backend *)backend;
    void *decmpbuf;
    size_t decmpbuflen;
    qgit_rawobj *rawobj;

    if (qgit_loose_backend_path(loose_backend->objects_dir, oid, path,
                                sizeof(path)) < 0)
        return -1;

    if (zlib_decompressf(path, &decmpbuf, &decmpbuflen) < 0) {
        if (errno == ENOENT) {
            qgit_seterror(QGITERR_OBJNOTFOUND);
            return -1;
        }
        return -1;
    }

    if (qgit_rawobj_parse(&rawobj, decmpbuf, decmpbuflen) < 0) {
        free(decmpbuf);
        return -1;
    }

    free(decmpbuf);

    *data_p = rawobj->data; /* transfer ownership */
    *len_p = rawobj->len;
    *type_p = rawobj->type;

    rawobj->data = NULL;
    qgit_rawobj_free(rawobj);

    return 0;
}
