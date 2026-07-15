/* miniutils - A minimal GNU coreutils implementation
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

#include <errno.h>
#include <fcntl.h>
#include <fileutils.h>
#include <limits.h>
#include <stdio.h>
#include <unistd.h>

int write_file(const char *path, const void *buf, size_t buflen)
{
    int fd;
    char tmppath[PATH_MAX];

    if (buflen == 0 || !buf) {
        errno = EINVAL;
        return -1;
    }

    if (snprintf(tmppath, PATH_MAX, "%s.tmp", path) >= PATH_MAX) {
        errno = ENAMETOOLONG;
        return -1;
    }

    if ((fd = open(tmppath, O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1)
        return -1;

    if (write_all(fd, buf, buflen) != (ssize_t)buflen) {
        close(fd);
        unlink(tmppath);
        return -1;
    }

    if (close(fd) == -1) {
        unlink(tmppath);
        return -1;
    }

    if (rename(tmppath, path) == -1) {
        unlink(tmppath);
        return -1;
    }

    return 0;
}