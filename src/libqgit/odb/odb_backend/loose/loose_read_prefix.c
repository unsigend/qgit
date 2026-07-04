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

#include "libqgit/oid.h"
#include "loose_backend.h"

#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <fs.h>
#include <libqgit/db/odb_backend.h>
#include <libqgit/error.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

int qgit_loose_backend_read_prefix(qgit_oid *full_oid, void **data_p,
                                   size_t *len_p, qgit_obj_type *type_p,
                                   struct qgit_odb_backend *backend,
                                   const qgit_oid *short_id, unsigned int len)
{
    assert(full_oid && data_p && len_p && type_p && backend && short_id &&
           len >= QGIT_OID_MINPREFIXLEN);

    char dirpath[PATH_MAX];
    char hex[QGIT_OID_HEXSZ + 1], short_hex[QGIT_OID_HEXSZ + 1];
    struct dirent *ent;
    int found = 0;
    struct qgit_loose_backend *loose_backend =
        (struct qgit_loose_backend *)backend;
    DIR *dir;

    qgit_oid_fmt(short_hex, short_id);
    short_hex[QGIT_OID_HEXSZ] = '\0';

    if (snprintf(dirpath, PATH_MAX, "%s/%c%c", loose_backend->objects_dir,
                 short_hex[0], short_hex[1]) >= PATH_MAX) {
        errno = ENAMETOOLONG;
        return -1;
    }

    if (!(dir = opendir(dirpath)))
        return -1;

    hex[0] = short_hex[0];
    hex[1] = short_hex[1];
    memcpy(hex + 2, short_hex + 2, len - 2);

    while ((ent = readdir(dir))) {
        if (strcmp(ent->d_name, "..") == 0 || strcmp(ent->d_name, ".") == 0)
            continue;
        if (strlen(ent->d_name) == QGIT_OID_HEXSZ - 2 &&
            strncmp(ent->d_name, hex + 2, len - 2) == 0) {
            if (found) {
                qgit_seterror(QGITERR_AMBIGUOUS);
                closedir(dir);
                return -1;
            }
            found = 1;
            memcpy(hex + 2, ent->d_name, QGIT_OID_HEXSZ - 2);
            hex[QGIT_OID_HEXSZ] = '\0';
        }
    }

    closedir(dir);

    if (!found) {
        qgit_seterror(QGITERR_OBJNOTFOUND);
        return -1;
    }

    if (qgit_oid_fromstr(full_oid, hex) < 0)
        return -1;

    return qgit_loose_backend_read(data_p, len_p, type_p, backend, full_oid);
}
