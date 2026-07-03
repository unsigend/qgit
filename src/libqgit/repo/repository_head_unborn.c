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

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

int qgit_repository_head_unborn(qgit_repository *repo)
{
    assert(repo);

    char *buf;
    size_t buflen;
    char ref[PATH_MAX];
    char refpath[PATH_MAX];

    if (repository_read_head(repo, &buf, &buflen))
        return -1;

    if (!buf || !buflen)
        return 0;

    if (sscanf(buf, "ref: %s", ref) != 1) {
        free(buf);
        return 0;
    }

    if (snprintf(refpath, PATH_MAX, "%s/%s", repo->repodir, ref) >= PATH_MAX) {
        errno = ENAMETOOLONG;
        free(buf);
        return -1;
    }

    int exists = file_exists(refpath);

    free(buf);

    return !exists;
}
