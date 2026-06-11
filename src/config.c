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
#include <stdlib.h>
#include <unistd.h>

#include "iniparse.h"
#include "repo.h"

int config_global_path(char *path)
{
  char *home = getenv("HOME");
  if (!home)
    return -1;

  if (snprintf(path, PATH_MAX, "%s/.qgitconfig", home) >= PATH_MAX) {
    errno = ENAMETOOLONG;
    return -1;
  }

  return 0;
}

struct iniFILE *config_global(void)
{
  char path[PATH_MAX];
  if (config_global_path(path) == -1)
    return NULL;

  struct iniFILE *fp = iniparse_open(path);
  if (!fp)
    return NULL;
  if (iniparse_parse(fp) == -1) {
    iniparse_close(fp);
    return NULL;
  }
  return fp;
}

struct iniFILE *config_cwd(void)
{
  char cwd[PATH_MAX];
  if (getcwd(cwd, sizeof(cwd)) == NULL)
    return NULL;

  struct repo *repo = repo_find(cwd);
  if (!repo)
    return NULL;

  char path[PATH_MAX];
  if (snprintf(path, sizeof(path), "%s/config", repo->gitdir) >= PATH_MAX) {
    repo_free(repo);
    errno = ENAMETOOLONG;
    return NULL;
  }

  struct iniFILE *fp = iniparse_open(path);
  if (!fp) {
    repo_free(repo);
    return NULL;
  }

  if (iniparse_parse(fp) == -1) {
    iniparse_close(fp);
    repo_free(repo);
    return NULL;
  }

  repo_free(repo);
  return fp;
}