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
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "compress.h"
#include "fs.h"
#include "obj/obj.h"
#include "sha1.h"

struct obj *obj_open(struct repo *repo, const unsigned char *sha1)
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

  if (strcmp(rawbuf, "blob") == 0)
    obj->type = OBJ_BLOB;
  else if (strcmp(rawbuf, "commit") == 0)
    obj->type = OBJ_COMMIT;
  else if (strcmp(rawbuf, "tree") == 0)
    obj->type = OBJ_TREE;
  else if (strcmp(rawbuf, "tag") == 0)
    obj->type = OBJ_TAG;
  else {
    errno = EINVAL;
    obj_close(obj);
    free(rawbuf);
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

  obj->payload = malloc(obj->payloadsz);
  if (!obj->payload) {
    free(rawbuf);
    obj_close(obj);
    return NULL;
  }
  memcpy(obj->payload, cursor, obj->payloadsz);
  memcpy(obj->sha1, sha1, SHA1_DIGEST_LENGTH);
  free(rawbuf);
  return obj;
}

void obj_close(struct obj *obj)
{
  if (!obj)
    return;

  if (obj->payload)
    free(obj->payload);
  free(obj);
}

static const char *obj_type_str(obj_type_t type)
{
  switch (type) {
  case OBJ_BLOB:
    return "blob";
  case OBJ_COMMIT:
    return "commit";
  case OBJ_TREE:
    return "tree";
  case OBJ_TAG:
    return "tag";
  default:
    errno = EINVAL;
    return NULL;
  }
}

int obj_write(struct repo *repo, struct obj *obj)
{
  if (!repo || !obj) {
    errno = EINVAL;
    return -1;
  }

  unsigned char hex[SHA1_HEX_LENGTH];
  if (sha1_to_hex(obj->sha1, hex) == -1)
    return -1;

  char path[PATH_MAX];
  if (snprintf(path, sizeof(path), "%s/objects/%c%c", repo->gitdir, hex[0],
               hex[1]) >= PATH_MAX) {
    errno = ENAMETOOLONG;
    return -1;
  }
  if (!dir_exists(path)) {
    mode_t dirmode = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
    if (mkdirp(path, dirmode) == -1)
      return -1;
  }
  if (snprintf(path, sizeof(path), "%s/objects/%c%c/%s", repo->gitdir, hex[0],
               hex[1], hex + 2) >= PATH_MAX) {
    errno = ENAMETOOLONG;
    return -1;
  }

  const char *type = obj_type_str(obj->type);
  if (!type)
    return -1;
  int nw = 0;
  nw = snprintf(NULL, 0, "%zu", obj->payloadsz);
  if (nw < 0) {
    errno = EINVAL;
    return -1;
  }
  /* obj format: <type> <payloadsz>\0<payload> */
  size_t rawbuflen = strlen(type) + 2 + nw + obj->payloadsz;
  void *rawbuf = malloc(rawbuflen);
  if (!rawbuf)
    return -1;
  unsigned char *cursor = (unsigned char *)rawbuf;

  if ((nw = snprintf(rawbuf, rawbuflen, "%s %zu", type, obj->payloadsz)) < 0) {
    free(rawbuf);
    return -1;
  }
  cursor += nw;
  *cursor++ = '\0';
  if (obj->payloadsz > 0)
    memcpy(cursor, obj->payload, obj->payloadsz);

  void *buf;
  size_t buflen;
  if (zlib_compress(rawbuf, rawbuflen, &buf, &buflen) == -1) {
    free(rawbuf);
    return -1;
  }

  free(rawbuf);

  mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
  int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, mode);
  if (fd == -1) {
    free(buf);
    return -1;
  }

  if (write_all(fd, buf, buflen) == -1) {
    close(fd);
    free(buf);
    return -1;
  }
  close(fd);
  free(buf);
  return 0;
}