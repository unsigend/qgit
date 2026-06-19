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
#include <unistd.h>

#include "fs.h"
#include "ref.h"
#include "repo.h"
#include "sha1.h"

int ref_resolve_path(struct repo *repo, const char *path, unsigned char *sha1)
{
  if (!repo || !path || !sha1)
    return -1;

  char buf[PATH_MAX];
  int fd = -1;
  unsigned char hex[SHA1_HEXLEN];

  if (snprintf(buf, PATH_MAX, "%s/%s", repo->qgitdir, path) >= PATH_MAX) {
    errno = ENAMETOOLONG;
    return -1;
  }

  if ((fd = open(buf, O_RDONLY)) == -1)
    return -1;

  if (read_all(fd, hex, SHA1_HEXLEN) != SHA1_HEXLEN) {
    close(fd);
    return -1;
  }

  close(fd);
  if (hex_to_sha1(hex, sha1) == -1)
    return -1;
  return 0;
}