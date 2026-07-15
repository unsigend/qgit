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

#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <assert.h>
#include <errno.h>
#include <fileutils.h>
#include <libqgit/common.h>
#include <libqgit/types.h>
#include <limits.h>
#include <stdio.h>

struct qgit_repository {
    char *workdir;
    char *repodir;

    qgit_odb *odb;
    qgit_index *index;
    qgit_config *config;
};

/**
 * Read the head of the repository into a buffer.
 *
 * @param repo the repository to read the head from
 * @param out_buf output pointer to receive the buffer, must not be NULL. Caller
 *                must free it.
 * @param out_buflen the length of the buffer, must not be NULL
 * @return 0 on success, -1 on error and set errno
 */
QGIT_INLINE(int)
repository_read_head(qgit_repository *repo, char **out_buf, size_t *out_buflen)
{
    assert(repo && out_buf && out_buflen);

    char path[PATH_MAX];
    if (snprintf(path, sizeof(path), "%s/HEAD", repo->repodir) >= PATH_MAX) {
        errno = ENAMETOOLONG;
        return -1;
    }

    if (read_file(path, (void **)out_buf, out_buflen))
        return -1;

    return 0;
}

#endif