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

#include <assert.h>
#include <errno.h>
#include <libqgit/db/odb_backend.h>
#include <stdio.h>

int qgit_loose_backend_path(const char *objects_dir, const qgit_oid *oid,
                            char *path, size_t pathlen)
{
    assert(objects_dir && oid && path && pathlen);

    char hex[QGIT_OID_HEXSZ + 1];
    qgit_oid_fmt(hex, oid);
    hex[QGIT_OID_HEXSZ] = '\0';

    if (snprintf(path, pathlen, "%s/%c%c/%s", objects_dir, hex[0], hex[1],
                 hex + 2) >= (int)pathlen) {
        errno = ENAMETOOLONG;
        return -1;
    }

    return 0;
}