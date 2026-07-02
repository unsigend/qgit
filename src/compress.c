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

#include <compress.h>
#include <errno.h>
#include <fcntl.h>
#include <fs.h>
#include <stdlib.h>
#include <unistd.h>
#include <zlib.h>

int zlib_compress(const void *src, size_t srclen, void **dest, size_t *destlen)
{
    uLongf buflen = compressBound(srclen);
    void *buf = malloc(buflen);
    if (!buf)
        return -1;

    int ret = compress2(buf, &buflen, src, srclen, Z_BEST_COMPRESSION);
    if (ret != Z_OK) {
        if (ret == Z_MEM_ERROR)
            errno = ENOMEM;
        else if (ret == Z_BUF_ERROR)
            errno = EINVAL;
        else
            errno = EIO;
        free(buf);
        return -1;
    }

    void *newbuf = realloc(buf, buflen); /* shrink to fit */
    if (!newbuf) {
        free(buf);
        return -1;
    }
    buf = newbuf;

    *dest = buf;
    *destlen = buflen;
    return 0;
}

int zlib_compressf(const void *src, size_t srclen, const char *path)
{
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    int fd = -1;
    void *dest = NULL;
    size_t destlen = 0;

    if (zlib_compress(src, srclen, &dest, &destlen) == -1)
        return -1;

    if ((fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, mode)) == -1) {
        free(dest);
        return -1;
    }

    if (write_all(fd, dest, destlen) == -1) {
        free(dest);
        close(fd);
        unlink(path);
        return -1;
    }
    free(dest);
    close(fd);
    return 0;
}

int zlib_decompress(const void *src, size_t srclen, void **dest,
                    size_t *destlen)
{
    uLongf buflen = srclen * 4;
    void *buf = malloc(buflen);
    if (!buf)
        return -1;

    int ret;
    while ((ret = uncompress(buf, &buflen, src, srclen)) == Z_BUF_ERROR) {
        buflen *= 2;
        void *newbuf = realloc(buf, buflen);
        if (!newbuf) {
            free(buf);
            return -1;
        }
        buf = newbuf;
    }

    if (ret != Z_OK) {
        if (ret == Z_MEM_ERROR)
            errno = ENOMEM;
        else
            errno = EIO;
        free(buf);
        return -1;
    }

    void *newbuf = realloc(buf, buflen); /* shrink to fit */
    if (!newbuf) {
        free(buf);
        return -1;
    }
    buf = newbuf;

    *dest = buf;
    *destlen = buflen;
    return 0;
}

int zlib_decompressf(const char *path, void **dest, size_t *destlen)
{
    struct stat st;
    int fd = -1;
    void *src = NULL;
    ssize_t n;
    size_t srclen = 0;

    if (stat(path, &st) == -1)
        return -1;
    if (!S_ISREG(st.st_mode)) {
        errno = EINVAL;
        return -1;
    }

    if (st.st_size == 0) { /* Guard against empty files */
        errno = EINVAL;
        return -1;
    }
    srclen = st.st_size;

    if ((fd = open(path, O_RDONLY)) == -1)
        return -1;

    src = malloc(srclen);
    if (!src) {
        close(fd);
        return -1;
    }

    if ((n = read_all(fd, src, srclen)) != (ssize_t)srclen) {
        if (n >= 0 && n < (ssize_t)srclen)
            errno = EIO;
        close(fd);
        free(src);
        return -1;
    }

    close(fd);
    if (zlib_decompress(src, srclen, dest, destlen) == -1) {
        free(src);
        return -1;
    }

    free(src);
    return 0;
}