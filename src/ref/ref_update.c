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
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "fs.h"
#include "ref.h"
#include "repo.h"
#include "sha1.h"

static mode_t dirmode = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
static mode_t filemode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

int ref_update(struct repo *repo, const char *refname,
               const unsigned char *sha1)
{
  if (!repo || !refname || !sha1 || !*refname)
    return -1;

  char buf[PATH_MAX], tmp[PATH_MAX];
  int fd = -1;
  unsigned char hex[SHA1_HEXLEN];
  char *slash = NULL;

  if (snprintf(buf, PATH_MAX, "%s/%s", repo->qgitdir, refname) >= PATH_MAX) {
    errno = ENAMETOOLONG;
    return -1;
  }

  slash = strrchr(buf, '/');
  if (!slash) {
    errno = EINVAL;
    return -1;
  }
  *slash = '\0';

  if (!dir_exists(buf)) {
    if (mkdirp(buf, dirmode) == -1)
      return -1;
  }

  *slash = '/';

  if (snprintf(tmp, PATH_MAX, "%s.tmp", buf) >= PATH_MAX) {
    errno = ENAMETOOLONG;
    return -1;
  }

  if ((fd = open(tmp, O_WRONLY | O_CREAT | O_TRUNC, filemode)) == -1)
    return -1;

  if (sha1_to_hex(sha1, hex) == -1) {
    unlink(tmp);
    close(fd);
    return -1;
  }

  if (write_all(fd, hex, SHA1_HEXLEN - 1) != SHA1_HEXLEN - 1) {
    unlink(tmp);
    close(fd);
    return -1;
  }

  if (write(fd, "\n", 1) != 1) {
    unlink(tmp);
    close(fd);
    return -1;
  }

  close(fd);
  if (rename(tmp, buf) == -1) {
    unlink(tmp);
    return -1;
  }
  return 0;
}