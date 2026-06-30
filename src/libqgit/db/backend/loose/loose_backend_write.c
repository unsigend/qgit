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
#include <errno.h>
#include <fs.h>
#include <libqgit/common.h>
#include <libqgit/db/oid.h>
#include <libqgit/object/object.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <util/compress.h>
#include <util/sha1.h>

int loose_backend_write(qgit_oid *out, qgit_odb_backend *backend,
                        const void *data, size_t len, qgit_obj_type type)
{
    assert(out && backend && type);

    struct loose_backend *loose_backend = (struct loose_backend *)backend;
    unsigned char sha[QGIT_OID_RAWSZ];
    void *buf;
    size_t buflen;
    char hex[QGIT_OID_HEXSZ], path1[PATH_MAX], path2[PATH_MAX];

    qgit_rawobj rawobj = {.data = (void *)data, .len = len, .type = type};

    if (qgit_rawobj_format(&rawobj, &buf, &buflen) == -1)
        return -1;

    if (sha1(buf, buflen, sha) == -1) {
        free(buf);
        return -1;
    }
    qgit_oid_fromraw(out, sha);
    if (qgit_oid_fmt(hex, out) == -1) {
        free(buf);
        return -1;
    }

    if (snprintf(path1, PATH_MAX, "%s/%c%c", loose_backend->objects_dir, hex[0],
                 hex[1]) >= PATH_MAX) {
        errno = ENAMETOOLONG;
        free(buf);
        return -1;
    }

    if (!dir_exists(path1)) {
        if (mkdirp(path1, QGIT_DIR_MODE) == -1) {
            free(buf);
            return -1;
        }
    }

    if (snprintf(path2, PATH_MAX, "%s/%s", path1, &hex[2]) >= PATH_MAX) {
        errno = ENAMETOOLONG;
        free(buf);
        return -1;
    }

    if (!file_exists(
            path2)) /* idempotent, skip write if the object already exists */
    {
        if (snprintf(path1, PATH_MAX, "%s.tmp", path2) >= PATH_MAX) {
            errno = ENAMETOOLONG;
            free(buf);
            return -1;
        }
        if (zlib_compressf(buf, buflen, path1) == -1) {
            free(buf);
            return -1;
        }
        if (rename(path1, path2) == -1) {
            unlink(path1); /* remove the tmp file */
            free(buf);
            return -1;
        }
    }

    free(buf);
    return 0;
}