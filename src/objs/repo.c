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
#include "iniparse.h"

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
  struct iniFILE *config = iniparse_create(buf);
  if (!config) {
    repo_close(repo);
    return NULL;
  }
  iniparse_set(config, "core", "repositoryformatversion", "0");
  iniparse_set(config, "core", "filemode", "true");
  iniparse_set(config, "core", "bare", repo->bare ? "true" : "false");

  if (iniparse_write(config) == -1) {
    iniparse_close(config);
    repo_close(repo);
    return NULL;
  }

  iniparse_close(config);

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
  free(repo);
}

struct repo *repo_find(const char *path)
{
  char abspath[PATH_MAX];
  char qgit[PATH_MAX];
  char parent[PATH_MAX];
  char absparent[PATH_MAX];
  struct stat st;

  if (!realpath(path, abspath))
    return NULL;

  while (1) {
    if (snprintf(qgit, PATH_MAX, "%s/.qgit", abspath) >= PATH_MAX)
      return NULL;
    if (stat(qgit, &st) == 0 && S_ISDIR(st.st_mode))
      return repo_open(abspath);

    if (snprintf(parent, PATH_MAX, "%s/..", abspath) >= PATH_MAX)
      return NULL;

    if (!realpath(parent, absparent))
      return NULL;
    if (strcmp(absparent, "/") == 0)
      return NULL;
    strcpy(abspath, absparent);
  }
}

char *repo_obj_path(struct repo *repo, const char *hash, char buf[PATH_MAX])
{
  if (!repo || !hash || !buf)
    return NULL;

  if (snprintf(buf, PATH_MAX, "%s/objects/%c%c/%s", repo->qgit, hash[0],
               hash[1], hash + 2) >= PATH_MAX)
    return NULL;
  return buf;
}