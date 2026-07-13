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
#include <libqgit/error.h>
#include <libqgit/repo/index.h>
#include <sha1.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* Parse an entry from the index file, return the address of the next entry */
static const char *parse_entry(qgit_index_entry *entry, const char *start,
                               const char *end)
{
    const char *p = start;
    uint16_t namelen; /* name length */
    uint16_t extened;
    size_t len;

    if (p + sizeof(uint32_t) * 10 > end) {
        qgit_seterror(QGITERR_BADINDEXFILE);
        return NULL;
    }

    entry->ctime.seconds = get_be32(p), p += 4;
    entry->ctime.nanoseconds = get_be32(p), p += 4;
    entry->mtime.seconds = get_be32(p), p += 4;
    entry->mtime.nanoseconds = get_be32(p), p += 4;
    entry->dev = get_be32(p), p += 4;
    entry->ino = get_be32(p), p += 4;
    entry->mode = get_be32(p), p += 4;
    entry->uid = get_be32(p), p += 4;
    entry->gid = get_be32(p), p += 4;
    entry->file_size = get_be32(p), p += 4;

    if (p + QGIT_OID_RAWSZ > end) {
        qgit_seterror(QGITERR_BADINDEXFILE);
        return NULL;
    }

    qgit_oid_fromraw(&entry->oid, (unsigned char *)p);
    p += QGIT_OID_RAWSZ;

    if (p + sizeof(uint16_t) > end) {
        qgit_seterror(QGITERR_BADINDEXFILE);
        return NULL;
    }

    entry->flags = get_be16(p), p += 2;
    namelen = entry->flags & QGIT_IDXENTRY_NAMEMASK;
    extened = entry->flags & 0x4000; /* extended flag */

    if (extened) {
        qgit_seterror(QGITERR_BADINDEXFILE);
        return NULL;
    }

    char *pathend = (char *)memchr(p, '\0', end - p);
    if (!pathend) {
        qgit_seterror(QGITERR_BADINDEXFILE);
        return NULL;
    }

    if ((namelen == 0xFFF && pathend - p < 0xFFF) ||
        (namelen < 0xFFF && pathend - p != namelen)) /* path length mismatch */
    {
        qgit_seterror(QGITERR_BADINDEXFILE);
        return NULL;
    }

    len = (namelen < 0xFFF) ? namelen : pathend - p;
    if (p + len + 1 > end) {
        qgit_seterror(QGITERR_BADINDEXFILE);
        return NULL;
    }
    entry->path = strndup(p, len);
    if (!entry->path)
        return NULL;
    p += len + 1; /* skip path and trailing null byte */

    /* align to the next multiple of QGIT_INDEX_ALIGNMENT */
    p = start + ALIGN(p - start, QGIT_INDEX_ALIGNMENT);
    if (p > end) {
        free(entry->path);
        qgit_seterror(QGITERR_BADINDEXFILE);
        return NULL;
    }

    return p;
}

int qgit_index_parse(qgit_index *index, const void *buf, size_t buflen)
{
    assert(index && buf);

    const char *p = buf;
    const char *end = p + buflen;
    const char *checksum = end - QGIT_OID_RAWSZ; /* checksum */
    uint32_t entry_count, ext_size;
    unsigned char computed_checksum[QGIT_OID_RAWSZ];

    if (buflen < QGIT_INDEX_HEADER_SIZE + QGIT_OID_RAWSZ) {
        qgit_seterror(QGITERR_BADINDEXFILE);
        return -1;
    }

    if (p + 12 > end) {
        qgit_seterror(QGITERR_BADINDEXFILE);
        return -1;
    }

    if (strncmp(p, QGIT_INDEX_MAGIC, 4) != 0) /* magic number */
    {
        qgit_seterror(QGITERR_BADINDEXMAGIC);
        return -1;
    }
    p += 4;

    index->version = get_be32(p);
    p += 4;
    if (index->version != QGIT_INDEX_VERSION) /* version */
    {
        qgit_seterror(QGITERR_BADINDEXVERSION);
        return -1;
    }

    entry_count = get_be32(p);
    p += 4;
    for (uint32_t i = 0; i < entry_count; i++) /* parse entries */
    {
        qgit_index_entry entry;
        p = parse_entry(&entry, p, checksum);

        if (!p)
            return -1;

        if (vec_pushback(index->entries, &entry) == -1) {
            free(entry.path);
            return -1;
        }
    }

    while (p < checksum) /* parse extensions */
    {
        if (p + 2 * sizeof(uint32_t) > checksum) {
            qgit_seterror(QGITERR_BADINDEXFILE);
            return -1;
        }

        ext_size = get_be32(p + 4);
        if (ext_size <= (size_t)(checksum - p - 8))
            p += 2 * sizeof(uint32_t) + ext_size; /* skip extension */
        else {
            qgit_seterror(QGITERR_BADINDEXFILE);
            return -1;
        }
        if (p > checksum) {
            qgit_seterror(QGITERR_BADINDEXFILE);
            return -1;
        }
    }

    if (p != checksum) {
        qgit_seterror(QGITERR_BADINDEXFILE);
        return -1;
    }

    if (sha1(buf, checksum - (const char *)buf, computed_checksum) < 0)
        return -1;
    if (memcmp(computed_checksum, checksum, QGIT_OID_RAWSZ) != 0) {
        qgit_seterror(QGITERR_BADINDEXCHECKSUM);
        return -1;
    }

    return 0;
}