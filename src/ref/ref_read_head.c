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
#include <sys/stat.h>
#include <unistd.h>

#include "fs.h"
#include "repo.h"

int ref_read_head(struct repo *repo, char *buf, size_t buflen)
{
  if (!repo || !buf || !buflen)
    return -1;

  char path[PATH_MAX];
  int fd = -1;
  int n = 0;
  struct stat st;

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

  if (st.st_size >= (long long)buflen) {
    errno = EOVERFLOW;
    close(fd);
    return -1;
  }

  n = read_all(fd, buf, buflen - 1);
  if (n <= 0) {
    close(fd);
    return -1;
  }
  close(fd);

  while (buf[n - 1] == '\n' || buf[n - 1] == '\r')
    n--;
  buf[n] = '\0';

  return 0;
}