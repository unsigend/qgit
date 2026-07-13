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

#include "index.h"
#include "stat.h"

#include <assert.h>
#include <errno.h>
#include <fileutil.h>
#include <libqgit/object/blob.h>
#include <libqgit/oid.h>
#include <libqgit/repo/index.h>
#include <libqgit/repo/repository.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

int qgit_index_add(qgit_index *index, const char *path, int stage)
{
    assert(index && path);

    char fullpath[PATH_MAX];
    struct stat st;
    qgit_index_entry entry;
    qgit_oid oid;
    size_t len = strlen(path);

    if (!index->owner) {
        errno = EINVAL;
        return -1;
    }

    if (snprintf(fullpath, PATH_MAX, "%s/%s",
                 qgit_repository_workdir(index->owner), path) >= PATH_MAX) {
        errno = ENAMETOOLONG;
        return -1;
    }

    if (!file_exists(fullpath)) {
        errno = ENOENT;
        return -1;
    }

    if (stat(fullpath, &st) < 0)
        return -1;

    if (qgit_blob_create_fromfile(&oid, index->owner, path) < 0)
        return -1;

    entry.ctime = (qgit_index_time){.seconds = st.st_ctime,
                                    .nanoseconds = stat_ctime_nsec(&st)};
    entry.mtime = (qgit_index_time){.seconds = st.st_mtime,
                                    .nanoseconds = stat_mtime_nsec(&st)};
    entry.dev = st.st_dev;
    entry.ino = st.st_ino;
    entry.mode = st.st_mode;
    entry.uid = st.st_uid;
    entry.gid = st.st_gid;
    entry.file_size = st.st_size;
    entry.oid = oid;
    entry.flags = (stage << QGIT_IDXENTRY_STAGESHIFT) |
                  (len < QGIT_IDXENTRY_NAMEMASK ? len : QGIT_IDXENTRY_NAMEMASK);
    entry.path = (char *)path;

    if (qgit_index_add2(index, &entry) < 0)
        return -1;

    return 0;
}
