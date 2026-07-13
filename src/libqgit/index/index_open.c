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
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

int qgit_index_open(qgit_index **out, const char *index_path)
{
    assert(out && index_path);
    *out = NULL;

    qgit_index *index = calloc(1, sizeof(qgit_index));
    void *buf = NULL;
    size_t buflen = 0;
    struct stat st;
    int fd;

    if (!index)
        return -1;

    index->path = strdup(index_path);
    if (!index->path) {
        qgit_index_free(index);
        return -1;
    }

    if (vec_init(&index->entries, sizeof(qgit_index_entry),
                 qgit_index_entry_free) < 0) {
        qgit_index_free(index);
        return -1;
    }

    if ((fd = open(index_path, O_RDONLY)) < 0) {
        if (errno == ENOENT) /* empty in-memory instance */
        {
            index->version = QGIT_INDEX_VERSION;
            *out = index;
            return 0;
        }
        qgit_index_free(index);
        return -1;
    }

    if (fstat(fd, &st) < 0) {
        close(fd);
        qgit_index_free(index);
        return -1;
    }

    buflen = st.st_size;
    if (!buflen) {
        close(fd);
        index->version = QGIT_INDEX_VERSION;
        *out = index;
        return 0;
    }

    if ((buf = mmap(NULL, buflen, PROT_READ, MAP_PRIVATE, fd, 0)) ==
        MAP_FAILED) {
        close(fd);
        qgit_index_free(index);
        return -1;
    }

    if (qgit_index_parse(index, buf, buflen) < 0) {
        munmap(buf, buflen);
        close(fd);
        qgit_index_free(index);
        return -1;
    }

    munmap(buf, buflen);
    close(fd);
    *out = index;
    return 0;
}
