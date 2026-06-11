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
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "obj/obj.h"

struct obj *obj_open_file(const char *path, obj_type_t type)
{
  if (!path) {
    errno = EINVAL;
    return NULL;
  }

  struct stat st;
  if (stat(path, &st) == -1)
    return NULL;
  if (!S_ISREG(st.st_mode)) {
    errno = EINVAL;
    return NULL;
  }

  int fd = open(path, O_RDONLY);
  if (fd == -1)
    return NULL;

  struct obj *obj = calloc(1, sizeof(struct obj));
  if (!obj) {
    close(fd);
    return NULL;
  }

  obj->type = type;
  obj->payloadsz = st.st_size;

  if (obj->payloadsz > 0) {
    void *buf = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (buf == MAP_FAILED) {
      close(fd);
      obj_close(obj);
      return NULL;
    }
    close(fd);

    obj->payload = malloc(st.st_size + 1);
    if (!obj->payload) {
      munmap(buf, st.st_size);
      obj_close(obj);
      return NULL;
    }
    memcpy(obj->payload, buf, st.st_size);
    ((char *)obj->payload)[st.st_size] = '\0';
    munmap(buf, st.st_size);
  } else {
    obj->payload = NULL;
    close(fd);
  }
  return obj;
}