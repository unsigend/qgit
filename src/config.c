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

#include "iniparse.h"

#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

struct iniFILE *config_global(void)
{
  char path[PATH_MAX];
  char *home = getenv("HOME");
  if (!home)
    return NULL;
  snprintf(path, PATH_MAX, "%s/.qgitconfig", home);
  struct stat st;
  if (stat(path, &st) == -1 || !S_ISREG(st.st_mode))
    return iniparse_create(path);
  struct iniFILE *config = iniparse_open(path);
  if (config && iniparse_parse(config) == -1) {
    iniparse_close(config);
    return NULL;
  }
  return config;
}

struct iniFILE *config_cwd(void)
{
  char path[PATH_MAX];
  char cwd[PATH_MAX];
  getcwd(cwd, PATH_MAX);
  if (snprintf(path, PATH_MAX, "%s/.qgit/config", cwd) >= PATH_MAX)
    return NULL;
  struct iniFILE *config = iniparse_open(path);
  if (config && iniparse_parse(config) == -1) {
    iniparse_close(config);
    return NULL;
  }
  return config;
}
