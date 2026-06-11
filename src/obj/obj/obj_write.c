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
#include <unistd.h>

#include "compress.h"
#include "fs.h"
#include "obj/obj.h"

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

  size_t rawbuflen;
  void *rawbuf = obj_build_rawbuf(obj, &rawbuflen);
  if (!rawbuf)
    return -1;

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