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
#include <stdlib.h>

#include "config.h"
#include "iniparse.h"

static const char *global_config_file = ".qgitconfig";

struct iniFILE *config_repo(struct repo *repo)
{
  if (!repo)
    return NULL;

  char buf[PATH_MAX];
  struct iniFILE *fp = NULL;

  if (snprintf(buf, PATH_MAX, "%s/config", repo->qgitdir) >= PATH_MAX) {
    errno = ENAMETOOLONG;
    return NULL;
  }

  if (!(fp = iniparse_open(buf)))
    return NULL;
  if (iniparse_parse(fp) == -1) {
    iniparse_close(fp);
    return NULL;
  }
  return fp;
}

struct iniFILE *config_global(void)
{
  char buf[PATH_MAX];
  char *home;
  struct iniFILE *fp = NULL;

  if (!(home = getenv("HOME"))) {
    if (!errno)
      errno = ENOENT;
    return NULL;
  }
  if (snprintf(buf, PATH_MAX, "%s/%s", home, global_config_file) >= PATH_MAX) {
    errno = ENAMETOOLONG;
    return NULL;
  }

  if (!(fp = iniparse_open(buf)))
    return NULL;
  if (iniparse_parse(fp) == -1) {
    iniparse_close(fp);
    return NULL;
  }
  return fp;
}

struct iniFILE *config_global_create(void)
{
  char buf[PATH_MAX];
  char *home;

  if (!(home = getenv("HOME"))) {
    if (!errno)
      errno = ENOENT;
    return NULL;
  }
  if (snprintf(buf, PATH_MAX, "%s/%s", home, global_config_file) >= PATH_MAX) {
    errno = ENAMETOOLONG;
    return NULL;
  }

  return iniparse_create(buf);
}