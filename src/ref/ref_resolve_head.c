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
#include <sys/stat.h>
#include <unistd.h>

#include "fs.h"
#include "ref.h"
#include "repo.h"
#include "sha1.h"

int ref_resolve_head(struct repo *repo, unsigned char *sha1)
{
  if (!repo || !sha1)
    return -1;

  char path[PATH_MAX];
  int fd = -1;
  struct stat st;
  char *buf;

  if (snprintf(path, PATH_MAX, "%s/HEAD", repo->qgitdir) >= PATH_MAX) {
    errno = ENAMETOOLONG;
    return -1;
  }

  if ((fd = open(path, O_RDONLY)) == -1)
    return -1;
  if (fstat(fd, &st) == -1) {
    close(fd);
    return -1;
  }

  if (!S_ISREG(st.st_mode)) {
    errno = EISDIR;
    close(fd);
    return -1;
  }

  if (!(buf = malloc(st.st_size + 1))) {
    close(fd);
    return -1;
  }
  if (read_all(fd, buf, st.st_size) != (ssize_t)st.st_size) {
    free(buf);
    close(fd);
    return -1;
  }
  close(fd);
  buf[st.st_size] = '\0';

  if (!strncmp(buf, "ref: ", 5)) /* symbolic HEAD */
  {
    char *refname = buf + 5;
    char *end = buf + st.st_size - 1;
    while (*end == '\n' || *end == '\r') /* strip trailing spaces */
      end--;
    *(end + 1) = '\0';
    if (ref_resolve_path(repo, refname, sha1) == -1) {
      free(buf);
      return -1;
    }
  } else /* detached HEAD */
  {
    if (hex_to_sha1((unsigned char *)buf, sha1) == -1) {
      free(buf);
      return -1;
    }
  }

  free(buf);
  return 0;
}