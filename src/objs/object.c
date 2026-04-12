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

#include "objs/object.h"
#include "compress.h"
#include "file.h"
#include "objs/repo.h"
#include "sha1.h"

#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#define OBJ_TYPE_LEN 8

static const char *obj_types[] = {
    [OBJ_COMMIT] = "commit",
    [OBJ_TREE] = "tree",
    [OBJ_BLOB] = "blob",
    [OBJ_TAG] = "tag",
};

/* Get the raw representation of the object: <type> <size>\0<payload>, return
   the buffer in heap. */
static char *object_raw(struct object *obj, int *sz)
{
  if (!obj || !sz)
    return NULL;

  *sz = 1;
  *sz += snprintf(NULL, 0, "%s %zu", obj_types[obj->type], obj->size);
  *sz += obj->size;

  char *raw = malloc(*sz);
  if (!raw)
    return NULL;

  char *buf = raw;
  buf += snprintf(buf, *sz, "%s %zu", obj_types[obj->type], obj->size);
  *buf++ = '\0';
  memcpy(buf, obj->payload, obj->size);

  return raw;
}

int object_hash(struct object *obj)
{
  if (!obj)
    return -1;

  int sz = 0;
  char *raw = object_raw(obj, &sz);
  if (!raw)
    return -1;

  unsigned char sha1[20];
  if (!sha1_hash((unsigned char *)raw, sz, sha1)) {
    free(raw);
    return -1;
  }
  sha1_hex(sha1, (unsigned char *)obj->sha1);
  free(raw);
  return 0;
}

struct object *object_read(struct repo *repo, const char *sha1)
{
  char path[PATH_MAX];
  if (!repo_obj_path(repo, sha1, path))
    return NULL;

  struct object *obj = malloc(sizeof(struct object));
  if (!obj)
    return NULL;
  memset(obj, 0, sizeof(struct object));
  strncpy(obj->sha1, sha1, 41);

  int fd = open(path, O_RDONLY);
  if (fd == -1) {
    object_free(obj);
    return NULL;
  }

  struct stat st;
  if (fstat(fd, &st) == -1) {
    close(fd);
    object_free(obj);
    return NULL;
  }

  size_t rawsz = st.st_size;
  unsigned char *raw = NULL;
  if ((raw = mmap(NULL, rawsz, PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED) {
    close(fd);
    object_free(obj);
    return NULL;
  }

  close(fd);

  unsigned char *dest = NULL;
  size_t destlen = 0;
  if (zlib_decompress(raw, rawsz, &dest, &destlen) == -1) {
    munmap(raw, rawsz);
    object_free(obj);
    return NULL;
  }
  munmap(raw, rawsz);

  char type[OBJ_TYPE_LEN];
  sscanf((const char *)dest, "%s %zu", type, &obj->size);
  if (strcmp(type, "commit") == 0)
    obj->type = OBJ_COMMIT;
  else if (strcmp(type, "tree") == 0)
    obj->type = OBJ_TREE;
  else if (strcmp(type, "blob") == 0)
    obj->type = OBJ_BLOB;
  else if (strcmp(type, "tag") == 0)
    obj->type = OBJ_TAG;
  else {
    free(dest);
    object_free(obj);
    return NULL;
  }

  unsigned char *payload = dest;
  while (*payload)
    ++payload;
  payload++;

  obj->payload = malloc(obj->size);
  if (!obj->payload) {
    free(dest);
    object_free(obj);
    return NULL;
  }

  memcpy(obj->payload, payload, obj->size);
  free(dest);

  return obj;
}

void object_free(struct object *obj)
{
  if (!obj)
    return;
  if (obj->payload)
    free(obj->payload);
  free(obj);
}

int object_write(struct repo *repo, struct object *obj)
{
  if (!repo || !obj)
    return -1;

  if (obj->sha1[0] == '\0' && object_hash(obj) == -1)
    return -1;

  int rawsz = 0;
  char *raw = object_raw(obj, &rawsz);
  if (!raw)
    return -1;

  unsigned char *dest = NULL;
  size_t destlen = 0;
  if (zlib_compress((unsigned char *)raw, rawsz, &dest, &destlen) == -1) {
    free(raw);
    return -1;
  }
  free(raw);

  char path[PATH_MAX];
  if (!repo_obj_path(repo, obj->sha1, path)) {
    free(dest);
    return -1;
  }

  char *dir = strrchr(path, '/');
  if (!dir) {
    free(dest);
    return -1;
  }
  *dir = '\0';
  if (mkdirp(path, PERM_DIR) == -1 && errno != EEXIST) {
    free(dest);
    return -1;
  }
  *dir = '/';

  int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, PERM_FILE);
  if (fd == -1) {
    free(dest);
    return -1;
  }
  if (write(fd, dest, destlen) != (ssize_t)destlen) {
    close(fd);
    free(dest);
    return -1;
  }

  close(fd);
  free(dest);

  return 0;
}

struct object *object_open(int type, const char *filename)
{
  if (!filename)
    return NULL;

  struct object *obj = malloc(sizeof(struct object));
  if (!obj)
    return NULL;
  memset(obj, 0, sizeof(struct object));
  obj->type = type;

  int fd = open(filename, O_RDONLY);
  if (fd == -1) {
    object_free(obj);
    return NULL;
  }
  struct stat st;
  if (fstat(fd, &st) == -1) {
    close(fd);
    object_free(obj);
    return NULL;
  }
  size_t rawsz = st.st_size;
  char *buf = mmap(NULL, rawsz, PROT_READ, MAP_PRIVATE, fd, 0);
  if (buf == MAP_FAILED) {
    close(fd);
    object_free(obj);
    return NULL;
  }
  close(fd);

  obj->payload = malloc(rawsz);
  obj->size = rawsz;
  if (!obj->payload) {
    munmap(buf, rawsz);
    object_free(obj);
    return NULL;
  }
  memcpy(obj->payload, buf, rawsz);
  munmap(buf, rawsz);
  return obj;
}