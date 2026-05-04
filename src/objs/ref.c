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

#include "objs/ref.h"
#include "objs/repo.h"

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/* Recursively read a ref from the repository, return 40 bytes SHA1 or NULL if
   failed. */
static char *ref_read(struct repo *repo, const char *refpath)
{
  char path[PATH_MAX];
  if (snprintf(path, PATH_MAX, "%s/%s", repo->qgit, refpath) >= PATH_MAX)
    return NULL;

  int fd = open(path, O_RDONLY);
  if (fd == -1)
    return NULL;

  char buf[512];
  ssize_t nread = read(fd, buf, sizeof(buf) - 1);
  close(fd);
  if (nread == -1 || nread == 0)
    return NULL;
  buf[nread] = '\0';

  if (strncmp(buf, "ref: ", 5) == 0) {
    char ref[PATH_MAX];
    sscanf(buf, "ref: %s", ref);
    return ref_read(repo, ref);
  }

  buf[strcspn(buf, "\n")] = '\0';
  return strdup(buf);
}

/* Resolve a ref to a SHA1, search order
   1. <name>
   2. refs/tags/<name>
   3. refs/heads/<name>
*/
char *ref_resolve(struct repo *repo, const char *ref)
{
  char *sha1 = NULL;
  if ((sha1 = ref_read(repo, ref)))
    return sha1;

  char path[PATH_MAX];

  if (snprintf(path, PATH_MAX, "refs/tags/%s", ref) >= PATH_MAX)
    return NULL;
  if ((sha1 = ref_read(repo, path)))
    return sha1;

  if (snprintf(path, PATH_MAX, "refs/heads/%s", ref) >= PATH_MAX)
    return NULL;
  if ((sha1 = ref_read(repo, path)))
    return sha1;

  return NULL;
}