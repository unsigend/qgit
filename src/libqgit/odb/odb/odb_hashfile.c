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

#include "odb.h"

#include <assert.h>
#include <fileutils.h>
#include <libqgit/db/odb.h>
#include <stdlib.h>

int qgit_odb_hashfile(qgit_oid *oid, const char *path, qgit_obj_type type)
{
    assert(oid && path && type);

    void *buf;
    size_t buflen;

    if (read_file(path, &buf, &buflen))
        return -1;

    if (qgit_odb_hash(oid, buf, buflen, type)) {
        free(buf);
        return -1;
    }

    free(buf);

    return 0;
}
