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

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "endian.h"
#include "error.h"
#include "index.h"
#include "sha1.h"

#define ALIGN(size, align) (((size) + (align - 1)) & ~(align - 1))

static const char *parse_entry(struct index_entry *entry, const char *buf,
                               const char *end)
{
  const char *start = buf;
  if (buf + sizeof(uint32_t) * 10 > end) {
    setqerrno(QE_BADIDX);
    return NULL;
  }
  entry->ctime = get_be32(buf), buf += 4;
  entry->ctime_nsec = get_be32(buf), buf += 4;
  entry->mtime = get_be32(buf), buf += 4;
  entry->mtime_nsec = get_be32(buf), buf += 4;
  entry->dev = get_be32(buf), buf += 4;
  entry->ino = get_be32(buf), buf += 4;
  entry->mode = get_be32(buf), buf += 4;
  entry->uid = get_be32(buf), buf += 4;
  entry->gid = get_be32(buf), buf += 4;
  entry->size = get_be32(buf), buf += 4;

  if (buf + SHA1_DIGLEN > end) {
    setqerrno(QE_BADIDX);
    return NULL;
  }
  sha1_copy((unsigned char *)buf, entry->sha1);
  buf += SHA1_DIGLEN;

  uint16_t flags = get_be16(buf);
  uint16_t namelen = flags & 0xFFF;
  entry->assume_valid = (flags >> 15) & 0x1;
  entry->extended = (flags >> 14) & 0x1;
  entry->stage = (flags >> 12) & 0x3;
  buf += 2;

  if (entry->extended) {
    setqerrno(QE_IDXSUPPORTS);
    return NULL;
  }

  const char *path = buf;
  size_t len = (namelen < 0xFFF) ? namelen : strlen(path);
  entry->path = strndup(path, len);
  if (!entry->path)
    return NULL;
  buf = path + len + 1; /* skip '\0' */

  buf = start + ALIGN(buf - start, IDXE_ALIGN); /* align to 8 bytes */
  if (buf > end) {
    setqerrno(QE_BADIDX);
    return NULL;
  }

  return buf;
}

int index_parse(struct index *index, const char *buf, size_t buflen)
{
  if (!index || !buf || buflen < IDX_HEADER_SIZE + SHA1_DIGLEN)
    return -1;

  const char *end = buf + buflen;
  const char *start = buf;
  const char *checksum = end - SHA1_DIGLEN;
  uint32_t n;

  /* parse header */
  if (strncmp(buf, IDX_SIGNATURE, 4) != 0) {
    setqerrno(QE_BADIDXMAGIC);
    return -1;
  }
  buf += 4;

  index->version = get_be32(buf);
  buf += 4;
  if (index->version != IDX_FMT_VERSION) {
    setqerrno(QE_BADIDXVERSION);
    return -1;
  }

  n = get_be32(buf);
  buf += 4;

  /* parse index entries */
  for (size_t i = 0; i < n; i++) {
    struct index_entry entry;
    const char *next = parse_entry(&entry, buf, checksum);
    if (!next)
      return -1;

    buf = next;
    if (vec_pushback(&index->entries, &entry) == -1) {
      free(entry.path);
      return -1;
    }
  }

  /* parse extension */
  while (buf < checksum) {
    if (buf + 8 > checksum) {
      setqerrno(QE_BADIDX);
      return -1;
    }

    /* in qgit, extension is ignored, and the parse logic is only for
       compatibility for git, in qgit it just ignore the extension and
       skip it */
    uint32_t extsz = get_be32(buf + 4); /* skip signature */
    buf += 8 + extsz;
    if (buf > checksum) {
      setqerrno(QE_BADIDX);
      return -1;
    }
  }

  /* parse checksum */
  if (buf != checksum) {
    setqerrno(QE_BADIDX);
    return -1;
  }
  unsigned char sha[SHA1_DIGLEN];

  if (sha1(start, checksum - start, sha) == -1)
    return -1;
  if (memcmp(sha, checksum, SHA1_DIGLEN) != 0) {
    setqerrno(QE_BADIDXCHECKSUM);
    return -1;
  }

  return 0;
}