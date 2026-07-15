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

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <libqgit/repo/index.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

int qgit_index_read(qgit_index *index)
{
    assert(index);

    int fd;
    struct stat st;
    size_t buflen = 0;
    void *buf = NULL;
    qgit_index tmp = {0};
    int err = 0;

    if (vec_init(&tmp.entries, sizeof(qgit_index_entry),
                 qgit_index_entry_free) < 0)
        return -1;

    if ((fd = open(index->path, O_RDONLY)) < 0) {
        err = errno;
        vec_free(tmp.entries);
        if (err == ENOENT) {
            qgit_index_clear(index);
            return 0;
        }
        return -1;
    }

    if (fstat(fd, &st) < 0) {
        close(fd);
        vec_free(tmp.entries);
        return -1;
    }

    buflen = st.st_size;
    if (!buflen) /* empty index file */
    {
        close(fd);
        vec_free(tmp.entries);
        qgit_index_clear(index);
        return 0;
    }

    if ((buf = mmap(NULL, buflen, PROT_READ, MAP_PRIVATE, fd, 0)) ==
        MAP_FAILED) {
        close(fd);
        vec_free(tmp.entries);
        return -1;
    }

    if (qgit_index_parse(&tmp, buf, buflen) < 0) {
        munmap(buf, buflen);
        close(fd);
        vec_free(tmp.entries);
        return -1;
    }

    munmap(buf, buflen);
    close(fd);

    qgit_index_clear(index);
    vec_free(index->entries);
    index->entries = tmp.entries;
    index->version = tmp.version;

    return 0;
}
