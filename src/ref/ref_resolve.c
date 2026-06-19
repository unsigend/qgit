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
#include <string.h>

#include "error.h"
#include "ref.h"
#include "repo.h"
#include "sha1.h"

int ref_resolve(struct repo *repo, const char *refname, unsigned char *sha1)
{
  if (!repo || !refname || !sha1)
    return -1;

  char buf[PATH_MAX];
  int found = 0;
  unsigned char tmp[SHA1_DIGLEN];

  /* HEAD */
  if (strcmp(refname, "HEAD") == 0)
    return ref_resolve_head(repo, sha1);

  /* full SHA1 */
  if (strlen(refname) == SHA1_HEXLEN - 1)
    return hex_to_sha1((unsigned char *)refname, sha1);

  /* full path */
  if (strncmp(refname, "refs/", 5) == 0)
    return ref_resolve_path(repo, refname, sha1);

  /* branches */
  if (snprintf(buf, PATH_MAX, "refs/heads/%s", refname) >= PATH_MAX) {
    errno = ENAMETOOLONG;
    return -1;
  }
  if (ref_resolve_path(repo, buf, sha1) != -1)
    found = 1;

  /* tags */
  if (snprintf(buf, PATH_MAX, "refs/tags/%s", refname) >= PATH_MAX) {
    errno = ENAMETOOLONG;
    return -1;
  }

  if (ref_resolve_path(repo, buf, tmp) != -1) {
    if (found && memcmp(tmp, sha1, SHA1_DIGLEN) != 0) { /* ambiguous */
      setqerrno(QE_AMBIGUOUS);
      return -1;
    }
    found = 1;
    sha1_copy(tmp, sha1);
  }

  if (found)
    return 0;

  /* short sha1 */
  return ref_resolve_short_sha1(repo, refname, sha1);
}