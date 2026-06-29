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
#include <libqgit/common.h>
#include <libqgit/object/object.h>
#include <limits.h>
#include <sha1.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int fmt_raw(void **buf, size_t *buflen, const void *data, size_t len,
                   qgit_obj_type type)
{
    int n = 0;
    const char *type_str = NULL;
    char *cur = NULL;

    if ((type_str = qgit_object_type2string(type)) == NULL)
        return -1;

    if ((n = snprintf(NULL, 0, "%s %zu", type_str, len)) < 0)
        return -1;

    *buflen = n + len + 1;
    if (!(*buf = malloc(*buflen)))
        return -1;

    cur = *buf;

    if ((n = snprintf(cur, *buflen, "%s %zu", type_str, len)) < 0) {
        free(*buf);
        *buf = NULL;
        return -1;
    }
    cur += n;

    *cur++ = '\0';
    if (data && len)
        memcpy(cur, data, len);

    return 0;
}

int loose_backend_write(qgit_oid *out, qgit_odb_backend *backend,
                        const void *data, size_t len, qgit_obj_type type)
{
    assert(out && backend && data && len && type);

    struct loose_backend *loose_backend = (struct loose_backend *)backend;
    void *buf = NULL;
    size_t buflen = 0;
    unsigned char sha[QGIT_OID_RAWSZ];
    char hex[QGIT_OID_HEXSZ], path1[PATH_MAX], path2[PATH_MAX];

    if (fmt_raw(&buf, &buflen, data, len, type) == -1)
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

    if (!file_exists(path2)) /* idempotent skip write  */
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