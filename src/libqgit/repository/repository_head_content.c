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
#include <errno.h>
#include <fcntl.h>
#include <fs.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int qgit_repository_head_content(const qgit_repository *repo, char *buf,
                                 size_t buflen)
{
    assert(repo && buf && buflen);

    char path[PATH_MAX];
    int fd = -1;

    if (snprintf(path, PATH_MAX, "%s/HEAD", repo->repodir) >= PATH_MAX) {
        errno = ENAMETOOLONG;
        return -1;
    }

    if ((fd = open(path, O_RDONLY)) == -1)
        return -1;

    if (read_all(fd, buf, buflen) < 0) {
        close(fd);
        return -1;
    }
    close(fd);

    return 0;
}