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

#include "endian.h"
#include "index.h"

#include <assert.h>
#include <errno.h>
#include <fileutils.h>
#include <libqgit/repo/index.h>
#include <sha1.h>
#include <stdlib.h>

/* Format the index entries into a buffer, if buffer is NULL or buflen is 0,
 * return the required number of bytes. */
static int index_entries_fmt(qgit_index *index, char *buf, size_t buflen)
{
    char *p = buf;
    int total = 0;

    for (size_t i = 0; i < vec_size(index->entries); i++) {
        qgit_index_entry *entry = (qgit_index_entry *)vec_at(index->entries, i);
        size_t len = strlen(entry->path);
        int n = ALIGN((sizeof(uint32_t) * 10 + len + 1 + QGIT_OID_RAWSZ +
                       sizeof(uint16_t)),
                      QGIT_INDEX_ALIGNMENT);
        if (buflen && buflen && (size_t)total + n > buflen) {
            errno = EOVERFLOW;
            return -1;
        }
        total += n;
        if (buf && buflen) {
            char *start = p;
            size_t padding;

            put_be32(p, entry->ctime.seconds), p += 4;
            put_be32(p, entry->ctime.nanoseconds), p += 4;
            put_be32(p, entry->mtime.seconds), p += 4;
            put_be32(p, entry->mtime.nanoseconds), p += 4;
            put_be32(p, entry->dev), p += 4;
            put_be32(p, entry->ino), p += 4;
            put_be32(p, entry->mode), p += 4;
            put_be32(p, entry->uid), p += 4;
            put_be32(p, entry->gid), p += 4;
            put_be32(p, entry->file_size), p += 4;
            memcpy(p, entry->oid.id, QGIT_OID_RAWSZ), p += QGIT_OID_RAWSZ;
            put_be16(p, entry->flags), p += 2;
            memcpy(p, entry->path, len), p += len;
            *p++ = '\0';
            padding = n - (p - start);
            memset(p, 0, padding);
            p += padding;
        }
    }
    return total;
}

int qgit_index_write(qgit_index *index)
{
    assert(index);

    qgit_index_sort(index);

    int total, nwrite;
    char *buf, *p;
    size_t buflen;
    unsigned char checksum[QGIT_OID_RAWSZ];

    if ((total = index_entries_fmt(index, NULL, 0)) < 0)
        return -1;

    buflen = total + QGIT_INDEX_HEADER_SIZE +
             QGIT_OID_RAWSZ; /* header + entries + checksum */
    buf = malloc(buflen);
    if (!buf)
        return -1;

    p = buf;
    memcpy(p, QGIT_INDEX_MAGIC, 4), p += 4;
    put_be32(p, index->version), p += 4;
    put_be32(p, vec_size(index->entries)), p += 4;

    nwrite = index_entries_fmt(index, p, total);
    if (nwrite < 0 || nwrite != total) {
        free(buf);
        return -1;
    }
    p += nwrite;

    if (sha1(buf, total + QGIT_INDEX_HEADER_SIZE, checksum) < 0) {
        free(buf);
        return -1;
    }
    memcpy(p, checksum, QGIT_OID_RAWSZ), p += QGIT_OID_RAWSZ;

    if (write_file(index->path, buf, buflen) < 0) {
        free(buf);
        return -1;
    }

    free(buf);

    return 0;
}
