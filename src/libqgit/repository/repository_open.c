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

#include "repository.h"

#include <errno.h>
#include <fs.h>
#include <libqgit/error.h>
#include <libqgit/repository.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int qgit_repository_open(qgit_repository **repo, const char *path)
{
    char repodir[PATH_MAX];
    struct qgit_repository *r = NULL;
    *repo = NULL;

    if (snprintf(repodir, PATH_MAX, "%s/.qgit", path) >= PATH_MAX) {
        errno = ENAMETOOLONG;
        return -1;
    }

    if (!dir_exists(repodir)) {
        qgit_seterrno(QGITERR_REPO_NOT_FOUND);
        return -1;
    }

    r = malloc(sizeof(struct qgit_repository));
    QGITERR_CHECK_ALLOC(r);
    memset(r, 0, sizeof(struct qgit_repository));

    r->repodir = strdup(repodir);
    if (!r->repodir) {
        qgit_repository_free(r);
        return -1;
    }

    r->workdir = strdup(path);
    if (!r->workdir) {
        qgit_repository_free(r);
        return -1;
    }

    /* TODO: load the configure file, object database, index, etc. */

    *repo = r;
    return 0;
}
