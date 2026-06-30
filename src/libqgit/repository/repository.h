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

#include <errno.h>
#include <libqgit/common.h>
#include <libqgit/config.h>
#include <libqgit/db/odb.h>
#include <libqgit/types.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>

struct qgit_repository {
    qgit_odb *odb;
    qgit_index *index;
    qgit_config *config;

    char *workdir;
    char *repodir;
};

/* Read the content of HEAD file in a repository to a buffer. */
QGIT_INTERNAL(int)
qgit_repository_head_content(const qgit_repository *repo, char *buf,
                             size_t buflen);

/* Load the config file of a repository. Returns -1 on error. */
QGIT_INLINE(int)
qgit_repository_load_config(const char *path, qgit_config **out)
{
    char buf[PATH_MAX];
    if (snprintf(buf, PATH_MAX, "%s/config", path) >= PATH_MAX) {
        errno = ENAMETOOLONG;
        return -1;
    }

    return qgit_config_open(out, buf);
}

/* Load the object database of a repository. Returns -1 on error. */
QGIT_INLINE(int)
qgit_repository_load_odb(const char *path, qgit_odb **out)
{
    char buf[PATH_MAX];
    if (snprintf(buf, PATH_MAX, "%s/objects", path) >= PATH_MAX) {
        errno = ENAMETOOLONG;
        return -1;
    }

    return qgit_odb_open(out, buf);
}
#endif