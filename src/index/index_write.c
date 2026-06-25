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

#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "endian.h"
#include "error.h"
#include "index.h"
#include "repo.h"
#include "sha1.h"

#define ALIGN(size, align) (((size) + (align - 1)) & ~(align - 1))

static int entry_cmp(const void *a, const void *b)
{
  const struct index_entry *e1 = (const struct index_entry *)a;
  const struct index_entry *e2 = (const struct index_entry *)b;
  int r = strcmp(e1->path, e2->path);
  if (r)
    return r;
  return (int)e1->stage - (int)e2->stage;
}

static int write_entry(struct index_entry *entry, void **buf, size_t *buflen)
{
  if (!entry || !buf || !buflen || !entry->path)
    return -1;

  size_t sz = 10 * sizeof(uint32_t) + SHA1_DIGLEN + sizeof(uint16_t) +
              strlen(entry->path) + 1;
  size_t aligned_sz = ALIGN(sz, IDXE_ALIGN);
  size_t npadding = aligned_sz - sz;

  *buflen = aligned_sz;
  *buf = malloc(aligned_sz);
  if (!*buf)
    return -1;

  char *p = *buf;
  size_t pathlen = strlen(entry->path);

  put_be32(p, entry->ctime);
  put_be32(p + 4, entry->ctime_nsec);
  put_be32(p + 8, entry->mtime);
  put_be32(p + 12, entry->mtime_nsec);
  put_be32(p + 16, entry->dev);
  put_be32(p + 20, entry->ino);
  put_be32(p + 24, entry->mode);
  put_be32(p + 28, entry->uid);
  put_be32(p + 32, entry->gid);
  put_be32(p + 36, entry->size);
  sha1_copy(entry->sha1, (unsigned char *)(p + 40));
  p += 10 * sizeof(uint32_t) + SHA1_DIGLEN;
  if (entry->extended) {
    free(*buf);
    setqerrno(QE_IDXSUPPORTS);
    return -1;
  }
  uint16_t flags = (entry->assume_valid << 15) | (entry->extended << 14) |
                   (entry->stage << 12);
  flags |= pathlen < 0xFFF ? pathlen : 0xFFF;
  put_be16(p, flags);
  p += sizeof(uint16_t);

  strcpy(p, entry->path);
  p += pathlen;
  *p++ = '\0';

  if (npadding)
    memset(p, 0, npadding);

  return 0;
}

int index_write(struct index *index)
{
  if (!index || !index->repo)
    return -1;

  vec_sort(&index->entries, entry_cmp);

  char path[PATH_MAX];
  char header[IDX_HEADER_SIZE];
  void *buf = NULL;
  size_t buflen = 0;
  FILE *fp = NULL;

  if (snprintf(path, PATH_MAX, "%s/index", index->repo->qgitdir) >= PATH_MAX) {
    errno = ENAMETOOLONG;
    return -1;
  }

  if (!(fp = fopen(path, "w")))
    return -1;

  /* write header */
  memcpy(header, IDX_SIGNATURE, 4);
  put_be32(header + 4, IDX_FMT_VERSION);
  put_be32(header + 8, (uint32_t)vec_size(&index->entries));
  if (fwrite(header, 1, IDX_HEADER_SIZE, fp) != IDX_HEADER_SIZE) {
    fclose(fp);
    return -1;
  }

  /* write entries */
  for (size_t i = 0; i < vec_size(&index->entries); i++) {
    struct index_entry *entry = vec_at(&index->entries, i);
    if (write_entry(entry, &buf, &buflen) == -1) {
      fclose(fp);
      return -1;
    }
    if (fwrite(buf, 1, buflen, fp) != buflen) {
      fclose(fp);
      free(buf);
      return -1;
    }

    free(buf);
    buf = NULL;
    buflen = 0;
  }

  fflush(fp);
  fseek(fp, 0, SEEK_END);
  long off = ftell(fp);
  if (off == -1) {
    fclose(fp);
    return -1;
  }
  size_t filesz = (size_t)off;

  buf = malloc(filesz);
  if (!buf) {
    fclose(fp);
    return -1;
  }

  rewind(fp); /* set offset to 0 */
  if (fread(buf, 1, filesz, fp) != filesz) {
    fclose(fp);
    free(buf);
    return -1;
  }

  fseek(fp, 0, SEEK_END);

  /* write checksum */
  unsigned char checksum[SHA1_DIGLEN];
  if (sha1(buf, filesz, checksum) == -1) {
    fclose(fp);
    free(buf);
    return -1;
  }

  if (fwrite(checksum, 1, SHA1_DIGLEN, fp) != SHA1_DIGLEN) {
    fclose(fp);
    free(buf);
    return -1;
  }

  free(buf);
  fclose(fp);
  return 0;
}