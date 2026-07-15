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
#include <fileutils.h>
#include <libqgit/db/odb_backend.h>
#include <libqgit/oid.h>
#include <limits.h>
#include <sha1.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

int qgit_loose_backend_write(qgit_oid *oid, struct qgit_odb_backend *backend,
                             const void *data, size_t len, qgit_obj_type type)
{
    assert(oid && backend);

    char path[PATH_MAX], dirpath[PATH_MAX];
    char hex[QGIT_OID_HEXSZ + 1];
    qgit_oid sha;
    struct qgit_loose_backend *loose_backend =
        (struct qgit_loose_backend *)backend;
    void *buf;
    size_t buflen;
    qgit_rawobj obj;

    obj.data = (void *)data;
    obj.len = len;
    obj.type = type;

    if (qgit_rawobj_fmt(&obj, &buf, &buflen) < 0)
        return -1;

    if (sha1(buf, buflen, sha.id)) {
        free(buf);
        return -1;
    }
    qgit_oid_cpy(oid, &sha);

    qgit_oid_fmt(hex, &sha);
    hex[QGIT_OID_HEXSZ] = '\0';

    if (snprintf(dirpath, PATH_MAX, "%s/%c%c", loose_backend->objects_dir,
                 hex[0], hex[1]) >= PATH_MAX) {
        free(buf);
        errno = ENAMETOOLONG;
        return -1;
    }
    if (!dir_exists(dirpath)) {
        if (mkdir(dirpath, 0755) < 0) {
            free(buf);
            return -1;
        }
    }

    if (snprintf(path, PATH_MAX, "%s/%s", dirpath, hex + 2) >= PATH_MAX) {
        free(buf);
        errno = ENAMETOOLONG;
        return -1;
    }

    if (zlib_compressf(buf, buflen, path)) {
        free(buf);
        return -1;
    }

    free(buf);
    return 0;
}
