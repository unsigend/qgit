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
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "compress.h"
#include "obj/obj.h"

struct obj *obj_open_sha1(struct repo *repo, const unsigned char *sha1)
{
  if (!repo || !sha1) {
    errno = EINVAL;
    return NULL;
  }

  unsigned char hex[SHA1_HEX_LENGTH];
  if (sha1_to_hex(sha1, hex) == -1)
    return NULL;

  char path[PATH_MAX];
  if (snprintf(path, sizeof(path), "%s/objects/%c%c/%s", repo->gitdir, hex[0],
               hex[1], hex + 2) >= PATH_MAX) {
    errno = ENAMETOOLONG;
    return NULL;
  }

  struct stat st;
  if (!(stat(path, &st) == 0 && S_ISREG(st.st_mode))) {
    errno = ENOENT;
    return NULL;
  }
  if (!st.st_size) {
    errno = EINVAL;
    return NULL;
  }

  int fd = open(path, O_RDONLY);
  if (fd == -1)
    return NULL;

  void *buf = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (buf == MAP_FAILED) {
    close(fd);
    return NULL;
  }
  close(fd);

  void *rawbuf;
  size_t rawbuflen;
  if (zlib_decompress(buf, st.st_size, &rawbuf, &rawbuflen) == -1) {
    munmap(buf, st.st_size);
    return NULL;
  }

  munmap(buf, st.st_size);

  unsigned char *cursor = rawbuf;
  unsigned char *end = (((unsigned char *)rawbuf) + rawbuflen);
  while (cursor < end && *cursor != ' ')
    cursor++;
  if (cursor == end) {
    free(rawbuf);
    return NULL;
  }
  *cursor++ = '\0';

  struct obj *obj = calloc(1, sizeof(struct obj));
  if (!obj) {
    free(rawbuf);
    return NULL;
  }

  obj->type = obj_type_from_str((char *)rawbuf);
  if (obj->type == OBJ_NONE) {
    free(rawbuf);
    obj_close(obj);
    return NULL;
  }

  char *endptr;
  errno = 0;
  obj->payloadsz = strtoul((char *)cursor, &endptr, 10);
  if (errno == ERANGE || errno == EINVAL || endptr == (char *)cursor ||
      *endptr != '\0') {
    free(rawbuf);
    obj_close(obj);
    return NULL;
  }
  cursor = (unsigned char *)endptr;
  if (cursor < end)
    cursor++;

  if (cursor + obj->payloadsz > end) {
    obj_close(obj);
    free(rawbuf);
    return NULL;
  }

  obj->payload = malloc(obj->payloadsz + 1);
  if (!obj->payload) {
    free(rawbuf);
    obj_close(obj);
    return NULL;
  }
  memcpy(obj->payload, cursor, obj->payloadsz);
  ((char *)obj->payload)[obj->payloadsz] = '\0';
  memcpy(obj->sha1, sha1, SHA1_DIGEST_LENGTH);
  free(rawbuf);
  return obj;
}
