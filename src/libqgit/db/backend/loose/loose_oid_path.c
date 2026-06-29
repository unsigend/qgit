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
#include <errno.h>
#include <libqgit/db/oid.h>
#include <limits.h>
#include <stdio.h>

int loose_oid_path(const char *objects_dir, const qgit_oid *oid, char *path,
                   size_t pathlen)
{
    assert(objects_dir && oid && path && pathlen);

    char oidpath[QGIT_OID_HEXSZ + 2];

    if (qgit_oid_fmtpath(oidpath, oid) == -1)
        return -1;

    if (snprintf(path, pathlen, "%s/%s", objects_dir, oidpath) >=
        (int)pathlen) {
        errno = ENAMETOOLONG;
        return -1;
    }

    return 0;
}