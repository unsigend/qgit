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

#include "repo.h"
#include "sha1.h"

FILE *repo_open_obj(struct repo *repo, unsigned char *sha1)
{
  if (!repo || !sha1)
    return NULL;

  char buf[PATH_MAX];
  unsigned char hex[SHA1_HEXLEN];
  FILE *fp = NULL;

  if (sha1_to_hex(sha1, hex) == -1)
    return NULL;

  if (snprintf(buf, PATH_MAX, "%s/objects/%c%c/%s", repo->qgitdir, hex[0],
               hex[1], &hex[2]) >= PATH_MAX) {
    errno = ENAMETOOLONG;
    return NULL;
  }

  if (!(fp = fopen(buf, "rb")))
    return NULL;

  return fp;
}