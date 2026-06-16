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
#include <sys/stat.h>
#include <unistd.h>

#include "fs.h"
#include "repo.h"
#include "sha1.h"

static int write_ref(const char *path, const unsigned char *sha1)
{
  int fd = -1;
  mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
  ssize_t n;

  fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, mode);
  if (fd == -1)
    return -1;

  unsigned char hex[SHA1_HEX_LENGTH];
  if (sha1_to_hex(sha1, hex) == -1) {
    close(fd);
    return -1;
  }

  n = write_all(fd, hex, SHA1_HEX_LENGTH - 1);
  if (n != SHA1_HEX_LENGTH - 1) {
    close(fd);
    return -1;
  }

  if (write(fd, "\n", 1) != 1) {
    close(fd);
    return -1;
  }

  close(fd);
  return 0;
}

int ref_update(struct repo *repo, const char *refname,
               const unsigned char *sha1)
{
  if (!repo || !refname || !sha1) {
    errno = EINVAL;
    return -1;
  }

  if (strncmp(refname, "refs/", 5) != 0) {
    errno = EINVAL;
    return -1;
  }

  char path[PATH_MAX];
  if (snprintf(path, PATH_MAX, "%s/%s", repo->gitdir, refname) >= PATH_MAX) {
    errno = ENAMETOOLONG;
    return -1;
  }

  /* Check whether the directory exists */
  char *slash = strrchr(path, '/');
  if (slash) {
    *slash = '\0';
    if (!dir_exists(path)) {
      mode_t mode = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
      if (mkdirp(path, mode) == -1)
        return -1;
    }
    *slash = '/';
  } else {
    errno = EINVAL;
    return -1;
  }

  return write_ref(path, sha1);
}