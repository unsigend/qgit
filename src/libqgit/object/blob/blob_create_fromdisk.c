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

#include <fileutils.h>
#include <libqgit/object/blob.h>

int qgit_blob_create_fromdisk(qgit_oid *oid, qgit_repository *repo,
                              const char *path)
{
    assert(oid && repo && path);

    void *buf;
    size_t len;

    if (read_file(path, &buf, &len) < 0)
        return -1;

    if (qgit_blob_create_frombuffer(oid, repo, buf, len) < 0) {
        free(buf);
        return -1;
    }

    free(buf);

    return 0;
}
