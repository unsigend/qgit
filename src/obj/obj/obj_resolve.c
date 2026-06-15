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
#include <stdio.h>
#include <string.h>

#include "obj/obj.h"
#include "ref.h"

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
  if (strcmp(name, "HEAD") == 0)
    return ref_read(repo, name, sha1);

  char buf[PATH_MAX];

  /* branch name */
  if (snprintf(buf, sizeof(buf), "refs/heads/%s", name) >= PATH_MAX) {
    errno = ENAMETOOLONG;
    return -1;
  }
  if (!ref_read(repo, buf, sha1))
    return 0;

  /* tag name */
  if (snprintf(buf, sizeof(buf), "refs/tags/%s", name) >= PATH_MAX) {
    errno = ENAMETOOLONG;
    return -1;
  }
  if (!ref_read(repo, buf, sha1))
    return 0;

  /* short sha1 hash */

  return -1;
}