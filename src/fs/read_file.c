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
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "fs.h"

int read_file(const char *path, void **buf, size_t *len)
{
    int fd = -1;
    struct stat st;
    void *tmpbuf = NULL;

    if (stat(path, &st) == -1)
        return -1;
    if (!S_ISREG(st.st_mode)) {
        errno = EINVAL;
        return -1;
    }

    if (st.st_size == 0) {
        *buf = NULL;
        *len = 0;
        return 0;
    }

    if ((fd = open(path, O_RDONLY)) == -1)
        return -1;

    tmpbuf = malloc(st.st_size + 1);
    if (!tmpbuf) {
        close(fd);
        return -1;
    }
    ((char *)tmpbuf)[st.st_size] = '\0';

    if (read_all(fd, tmpbuf, st.st_size) != (ssize_t)st.st_size) {
        free(tmpbuf);
        close(fd);
        return -1;
    }

    *buf = tmpbuf;
    *len = st.st_size;
    close(fd);
    return 0;
}