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
#include <string.h>
#include <unistd.h>

#include "fs.h"
#include "ref.h"
#include "sha1.h"

/* Read a SHA1 from a fd, return 0 on success, -1 on error and set errno. */
static int read_sha1(int fd, unsigned char *sha1)
{
  char buf[SHA1_HEX_LENGTH];
  ssize_t n = 0;
  if ((n = read_all(fd, buf, SHA1_HEX_LENGTH - 1)) < SHA1_HEX_LENGTH - 1)
    return -1;
  buf[SHA1_HEX_LENGTH - 1] = '\0';
  return hex_to_sha1((unsigned char *)buf, sha1);
}

static int resolve_head(struct repo *repo, unsigned char *sha1)
{
  char path[PATH_MAX];
  char buf[512];
  ssize_t n = 0;
  int fd = -1;

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

  if (strncmp(buf, "ref: ", 5) == 0) {
    char *target = buf + 5;
    size_t len = strlen(target);

    while (len > 0 && (target[len - 1] == '\n' || target[len - 1] == '\r'))
      target[--len] = '\0';

    return ref_read(repo, target, sha1);
  }

  return hex_to_sha1((unsigned char *)buf, sha1);
}

int ref_read(struct repo *repo, const char *refname, unsigned char *sha1)
{
  if (!repo || !refname || !sha1) {
    errno = EINVAL;
    return -1;
  }

  if (strncmp(refname, "refs/", 5) != 0 && strcmp(refname, "HEAD") != 0) {
    errno = EINVAL;
    return -1;
  }

  if (strcmp(refname, "HEAD") == 0)
    return resolve_head(repo, sha1);

  int fd = -1;
  char path[PATH_MAX];
  if (snprintf(path, PATH_MAX, "%s/%s", repo->gitdir, refname) >= PATH_MAX) {
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

  if (read_sha1(fd, sha1) == -1) {
    close(fd);
    return -1;
  }

  close(fd);
  return 0;
}