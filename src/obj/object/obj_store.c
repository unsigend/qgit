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
#include <stdio.h>

#include "compress.h"
#include "fs.h"
#include "obj/object.h"

static mode_t dirmode = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;

int obj_store(struct repo *repo, const unsigned char *sha1, const void *buf,
              size_t buflen)
{
  if (!repo || !sha1 || !buf || !buflen)
    return -1;

  unsigned char hex[SHA1_HEXLEN];
  char path[PATH_MAX];

  if (sha1_to_hex(sha1, hex) == -1)
    return -1;

  if (snprintf(path, PATH_MAX, "%s/objects/%c%c", repo->qgitdir, hex[0],
               hex[1]) >= PATH_MAX) {
    errno = ENAMETOOLONG;
    return -1;
  }

  if (!dir_exists(path)) {
    if (mkdirp(path, dirmode) == -1)
      return -1;
  }

  if (snprintf(path, PATH_MAX, "%s/objects/%c%c/%s", repo->qgitdir, hex[0],
               hex[1], &hex[2]) >= PATH_MAX) {
    errno = ENAMETOOLONG;
    return -1;
  }

  if (zlib_compressf(buf, buflen, path) == -1)
    return -1;

  return 0;
}