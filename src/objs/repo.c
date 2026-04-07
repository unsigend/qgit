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

#include "objs/repo.h"
#include "file.h"

#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

static const char *dirs[] = {
    "objects/info",
    "objects/pack",
    "refs/heads",
    "refs/tags",
};

struct repo *repo_open(const char *abspath)
{
  char buf[PATH_MAX];
  struct stat st;

  snprintf(buf, PATH_MAX, "%s/.qgit", abspath);
  if (stat(buf, &st) == -1 || !S_ISDIR(st.st_mode))
    return NULL;

  struct repo *repo = calloc(1, sizeof(struct repo));
  if (!repo)
    return NULL;

  repo->worktree = strdup(abspath);
  if (!repo->worktree) {
    repo_close(repo);
    return NULL;
  }

  repo->qgit = strdup(buf);
  if (!repo->qgit) {
    repo_close(repo);
    return NULL;
  }

  strcat(buf, "/config");
  repo->config = iniparse_open(buf);
  if (repo->config && iniparse_parse(repo->config) == -1) {
    iniparse_close(repo->config);
    repo_close(repo);
    repo->config = NULL;
    return NULL;
  }

  return repo;
}

struct repo *repo_create(const char *abspath, const char *bname, bool bare)
{
  char buf[PATH_MAX];
  struct repo *repo = calloc(1, sizeof(struct repo));
  if (!repo)
    return NULL;

  repo->worktree = strdup(abspath);
  if (!repo->worktree) {
    repo_close(repo);
    return NULL;
  }

  snprintf(buf, PATH_MAX, "%s/.qgit", abspath);
  repo->qgit = strdup(buf);
  if (!repo->qgit) {
    repo_close(repo);
    return NULL;
  }
  repo->bare = bare;

  struct stat st;
  if (!stat(repo->qgit, &st) && S_ISDIR(st.st_mode)) {
    repo->reinit = true;
    return repo;
  }

  for (size_t i = 0; i < sizeof(dirs) / sizeof(dirs[0]); i++) {
    snprintf(buf, PATH_MAX, "%s/%s", repo->qgit, dirs[i]);
    if (mkdirp(buf, PERM_DIR) == -1 && errno != EEXIST) {
      repo_close(repo);
      return NULL;
    }
  }

  snprintf(buf, PATH_MAX, "%s/description", repo->qgit);
  FILE *fp = fopen(buf, "w");
  if (!fp) {
    repo_close(repo);
    return NULL;
  }
  fprintf(fp, "Unnamed repository; edit this file 'description' to name the "
              "repository.\n");
  fclose(fp);

  snprintf(buf, PATH_MAX, "%s/HEAD", repo->qgit);
  fp = fopen(buf, "w");
  if (!fp) {
    repo_close(repo);
    return NULL;
  }
  fprintf(fp, "ref: refs/heads/%s\n", bname);
  fclose(fp);

  snprintf(buf, PATH_MAX, "%s/config", repo->qgit);
  fp = fopen(buf, "w");
  if (!fp) {
    repo_close(repo);
    return NULL;
  }
  fprintf(fp, "[core]\n");
  fprintf(fp, "\trepositoryformatversion = 0\n");
  fprintf(fp, "\tfilemode = true\n");
  fprintf(fp, "\tbare = %s\n", repo->bare ? "true" : "false");
  fclose(fp);

  return repo;
}

void repo_close(struct repo *repo)
{
  if (!repo)
    return;
  if (repo->worktree)
    free(repo->worktree);
  if (repo->qgit)
    free(repo->qgit);
  if (repo->config)
    iniparse_close(repo->config);
  free(repo);
}