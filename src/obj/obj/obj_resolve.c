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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "fs.h"
#include "obj/obj.h"
#include "sha1.h"

static int resolve_branch(struct repo *repo, const char *bname,
                          unsigned char *sha1)
{
  if (!repo || !bname || !sha1) {
    errno = EINVAL;
    return -1;
  }

  char path[PATH_MAX];
  int fd = -1;
  char buf[SHA1_HEX_LENGTH];
  ssize_t n = 0;

  if (snprintf(path, PATH_MAX, "%s/refs/heads/%s", repo->gitdir, bname) >=
      PATH_MAX) {
    errno = ENAMETOOLONG;
    return -1;
  }

  if (!file_exists(path)) {
    errno = ENOENT;
    return -1;
  }

  fd = open(path, O_RDONLY);
  if (fd == -1)
    return -1;

  if ((n = read_all(fd, buf, SHA1_HEX_LENGTH - 1)) < SHA1_HEX_LENGTH - 1) {
    close(fd);
    return -1;
  }

  buf[SHA1_HEX_LENGTH - 1] = '\0';
  close(fd);

  if (hex_to_sha1((unsigned char *)buf, sha1) == -1)
    return -1;

  return 0;
}

int obj_resolve(struct repo *repo, const char *name, unsigned char *sha1)
{
  if (!repo || !name || !sha1) {
    errno = EINVAL;
    return -1;
  }

  /* full sha1 hash */
  if (strlen(name) == SHA1_HEX_LENGTH - 1) {
    if (hex_to_sha1((unsigned char *)name, sha1) == -1)
      return -1;
    return 0;
  }

  /* HEAD reference */
  if (strcmp(name, "HEAD") == 0) {
    char path[PATH_MAX];
    int fd = -1;
    char buf[1024];
    ssize_t n = 0;
    char bname[64];

    if (snprintf(path, PATH_MAX, "%s/HEAD", repo->gitdir) >= PATH_MAX) {
      errno = ENAMETOOLONG;
      return -1;
    }

    if (!file_exists(path)) {
      errno = ENOENT;
      return -1;
    }

    fd = open(path, O_RDONLY);
    if (fd == -1)
      return -1;

    if ((n = read_all(fd, buf, sizeof(buf) - 1)) < 0) {
      close(fd);
      return -1;
    }

    buf[n] = '\0';
    close(fd);

    if (sscanf(buf, "ref: refs/heads/%63s", bname) != 1) {
      errno = EINVAL;
      return -1;
    }

    if (resolve_branch(repo, bname, sha1) == -1)
      return -1;
    return 0;
  }

  return -1;
}