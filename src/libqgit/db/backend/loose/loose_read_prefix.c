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

#include "libqgit/db/oid.h"
#include "loose_backend.h"

#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <libqgit/error.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

int loose_backend_read_prefix(qgit_oid *full_oid_out, void **data_p,
                              size_t *len_p, qgit_obj_type *type_p,
                              qgit_odb_backend *backend,
                              const qgit_oid *short_id, unsigned int len)
{
    assert(full_oid_out && data_p && len_p && type_p && backend && short_id &&
           len >= QGIT_OID_MINPREFIXLEN && len <= QGIT_OID_RAWSZ * 2);

    struct loose_backend *loose_backend = (struct loose_backend *)backend;
    char path[PATH_MAX];
    struct dirent *ent;
    DIR *dir = NULL;
    char short_hex[QGIT_OID_HEXSZ], full_hex[QGIT_OID_HEXSZ];
    int found = 0;

    qgit_oid_fmt(short_hex, short_id);
    short_hex[QGIT_OID_HEXSZ - 1] = '\0';

    if (snprintf(path, PATH_MAX, "%s/%.2s", loose_backend->objects_dir,
                 short_hex) >= PATH_MAX) {
        errno = ENAMETOOLONG;
        return -1;
    }

    if (!(dir = opendir(path)))
        return -1;

    full_hex[0] = short_hex[0];
    full_hex[1] = short_hex[1];

    while ((ent = readdir(dir)) != NULL) {
        if (strcmp(ent->d_name, "..") == 0 || strcmp(ent->d_name, ".") == 0)
            continue;
        if (strncmp(ent->d_name, short_hex + 2, len - 2) == 0) {
            if (found) /* ambiguous */
            {
                qgit_seterrno(QGITERR_AMBIGUOUS);
                closedir(dir);
                return -1;
            }
            found = 1;
            memcpy(full_hex + 2, ent->d_name, QGIT_OID_HEXSZ - 2);
        }
    }

    if (!found) {
        qgit_seterrno(QGITERR_OBJ_NOT_FOUND);
        closedir(dir);
        return -1;
    }

    qgit_oid_fromstr(full_oid_out, full_hex);

    closedir(dir);
    return loose_backend_read(data_p, len_p, type_p, backend, full_oid_out);
}