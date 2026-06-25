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

#ifndef INDEX_H
#define INDEX_H

#include <stdint.h>

#include "collection/vector.h"
#include "sha1.h"

/* Git index file format design: https://git-scm.com/docs/index-format.

   qgit index file binary format (big endian) v2

   header:
     4 bytes signature: {'D', 'I', 'R', 'C'} dircache
     4 bytes version (qgit use version 2)
     4 bytes number of entries (n)

    entries[n]:
     32-bit ctime seconds
     32-bit ctime nanoseconds
     32-bit mtime seconds
     32-bit mtime nanoseconds
     32-bit dev
     32-bit ino
     32-bit mode [object_type:4][unused:3][permission:9]
     32-bit uid
     32-bit gid
     32-bit file size
     20-bytes SHA1 hash of the object
     16-bit flags [assume_valid:1][extended:1][stage:2][namelen:12 | 0xFFF if
   length >= 0xFFF]
     (version >= 3 && extended == 1): 16-bit flags:
   [reserved:1][skip_worktree:1][intent_to_add:1][unused:13]
     path name (null terminated, relative no leading '/')
     1 - 8 NULL bytes padding to 8-byte alignment

    extension (unused in qgit)
     4 bytes signature
     4 bytes size of the extension
     extension data

    Hash checksum:
     over the content of the index file before this checksum (SHA1)
*/

#define IDX_SIGNATURE "DIRC"
#define IDX_HEADER_SIZE 12
#define IDX_FMT_VERSION 2

#define IDXE_ALIGN 8

#define IDXE_TYPE_REGULAR 0b1000
#define IDXE_TYPE_SYMLINK 0b1010
#define IDXE_TYPE_GITLINK 0b1110

struct repo;

struct index_entry {
  uint32_t ctime;
  uint32_t ctime_nsec;
  uint32_t mtime;
  uint32_t mtime_nsec;
  uint32_t dev;
  uint32_t ino;
  uint32_t mode;
  uint32_t uid;
  uint32_t gid;
  uint32_t size;                   /* file size */
  unsigned char sha1[SHA1_DIGLEN]; /* 20 bytes */
  uint16_t assume_valid : 1;
  uint16_t extended : 1; /* must be 0 in version 2 */
  uint16_t stage : 2;
  char *path; /* on heap */
};

struct index {
  struct vector entries;
  uint32_t version;
  struct repo *repo;
};

/* if not exists, create it and return empty index with v2*/
extern struct index *index_open(struct repo *repo);
extern int index_write(struct index *index); /* flush to disk */
extern void index_close(struct index *index);

extern struct index_entry *index_find(struct index *index, const char *path,
                                      uint16_t stage);
extern int index_add(struct index *index, struct index_entry *entry);
extern int index_rm(struct index *index, const char *path, uint16_t stage);

#endif