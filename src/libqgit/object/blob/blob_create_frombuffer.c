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

#include "blob.h"

#include <assert.h>
#include <errno.h>
#include <libqgit/repo/repository.h>
#include <limits.h>
#include <stdio.h>

int qgit_blob_create_frombuffer(qgit_oid *oid, qgit_repository *repo,
                                const void *buffer, size_t len)
{
    assert(oid && repo);

    qgit_odb *odb;
    char path[PATH_MAX];

    if (snprintf(path, PATH_MAX, "%s/objects", qgit_repository_path(repo)) >=
        PATH_MAX) {
        errno = ENAMETOOLONG;
        return -1;
    }

    if (qgit_odb_open(&odb, path) < 0)
        return -1;

    if (qgit_odb_write(oid, odb, buffer, len, QGIT_OBJ_BLOB) < 0) {
        qgit_odb_free(odb);
        return -1;
    }

    qgit_odb_free(odb);
    return 0;
}
