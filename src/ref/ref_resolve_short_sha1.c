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

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "error.h"
#include "ref.h"
#include "repo.h"
#include "sha1.h"

int ref_resolve_short_sha1(struct repo *repo, const char *refname,
                           unsigned char *sha1)
{
  if (!repo || !refname || !sha1)
    return -1;

  size_t len = strlen(refname);

  if (len < 7 || len > SHA1_HEXLEN - 1) {
    errno = EINVAL;
    return -1;
  }

  for (size_t i = 0; i < len; i++) { /* hex characters only */
    if (!isxdigit(refname[i])) {
      errno = EINVAL;
      return -1;
    }
  }

  char buf[PATH_MAX];
  DIR *dir = NULL;
  struct dirent *ent = NULL;
  char normname[SHA1_HEXLEN];
  int found = 0;
  unsigned char hex[SHA1_HEXLEN];
  const char *filename;

  for (size_t i = 0; i < len; i++) { /* normalize to lowercase */
    normname[i] = tolower(refname[i]);
  }
  normname[len] = '\0';
  filename = normname + 2;

  if (snprintf(buf, PATH_MAX, "%s/objects/%c%c", repo->qgitdir, normname[0],
               normname[1]) >= PATH_MAX) {
    errno = ENAMETOOLONG;
    return -1;
  }

  if (!(dir = opendir(buf)))
    return -1;

  while ((ent = readdir(dir)) != NULL) {
    if (!strncmp(ent->d_name, filename, strlen(filename))) {
      if (found) /* ambiguous */
      {
        setqerrno(QE_AMBIGUOUS);
        closedir(dir);
        return -1;
      }
      found = 1;
      if (snprintf((char *)hex, SHA1_HEXLEN, "%c%c%s", normname[0], normname[1],
                   ent->d_name) >= SHA1_HEXLEN) {
        closedir(dir);
        return -1;
      }
      if (hex_to_sha1(hex, sha1) == -1) {
        closedir(dir);
        return -1;
      }
    }
  }
  closedir(dir);
  if (!found) {
    errno = ENOENT;
    return -1;
  }
  return 0;
}