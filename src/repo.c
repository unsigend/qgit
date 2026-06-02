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
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "file.h"
#include "iniparse.h"
#include "repo.h"

#define DIR_PERM (S_IRWXU | S_IRWXG | S_IRWXO)

struct repo *repo_init(const char *path)
{
  if (!path || !*path) {
    errno = EINVAL;
    return NULL;
  }

  struct repo *repo = calloc(1, sizeof(struct repo));
  if (!repo)
    return NULL;

  repo->worktree = malloc(PATH_MAX);
  if (!repo->worktree) {
    repo_free(repo);
    return NULL;
  }
  if (strlen(path) >= PATH_MAX) {
    repo_free(repo);
    return NULL;
  }
  strcpy(repo->worktree, path);

  repo->gitdir = malloc(PATH_MAX);
  if (!repo->gitdir) {
    repo_free(repo);
    return NULL;
  }
  if (snprintf(repo->gitdir, PATH_MAX, "%s/.qgit", path) >= PATH_MAX) {
    repo_free(repo);
    return NULL;
  }

  return repo;
}

void repo_free(struct repo *repo)
{
  if (!repo)
    return;
  if (repo->worktree)
    free(repo->worktree);
  if (repo->gitdir)
    free(repo->gitdir);
  free(repo);
}

static const char *dirs[] = {"objects", "refs/heads", "refs/tags"};
static const size_t ndirs = sizeof(dirs) / sizeof(dirs[0]);

static void rollback(struct repo *repo)
{
  if (!repo || !repo->worktree || !repo->gitdir)
    return;
  rmrf(repo->gitdir);
}

int repo_create(struct repo *repo, const char *branch)
{
  if (!repo || !repo->worktree || !repo->gitdir)
    return -1;
  if (!branch || !*branch)
    return -1;

  if (mkdirp(repo->worktree, DIR_PERM) == -1)
    goto rollback;
  if (mkdirp(repo->gitdir, DIR_PERM) == -1)
    goto rollback;

  for (size_t i = 0; i < ndirs; i++) {
    char dir[PATH_MAX];
    if (snprintf(dir, sizeof(dir), "%s/%s", repo->gitdir, dirs[i]) >= PATH_MAX)
      goto rollback;

    if (mkdirp(dir, DIR_PERM) == -1)
      goto rollback;
  }

  char filename[PATH_MAX];
  FILE *fp = NULL;

  /* HEAD */
  if (snprintf(filename, PATH_MAX, "%s/HEAD", repo->gitdir) >= PATH_MAX)
    goto rollback;
  if (!file_exists(filename)) {
    fp = fopen(filename, "w");
    if (!fp)
      goto rollback;
    if (fprintf(fp, "ref: refs/heads/%s\n", branch) < 0) {
      fclose(fp);
      goto rollback;
    }
    fclose(fp);
  }

  /* description */
  if (snprintf(filename, PATH_MAX, "%s/description", repo->gitdir) >= PATH_MAX)
    goto rollback;

  if (!file_exists(filename)) {
    fp = fopen(filename, "w");
    if (!fp)
      goto rollback;
    if (fprintf(fp,
                "Unnamed repository; edit this file 'description' to name the "
                "repository.\n") < 0) {
      fclose(fp);
      goto rollback;
    }
    fclose(fp);
  }

  /* config */
  if (snprintf(filename, PATH_MAX, "%s/config", repo->gitdir) >= PATH_MAX)
    goto rollback;

  if (!file_exists(filename)) {
    struct iniFILE *config = iniparse_create(filename);
    if (!config)
      goto rollback;

    int ret = 0;
    ret |= iniparse_set(config, "core", "repositoryformatversion", "0");
    ret |= iniparse_set(config, "core", "filemode", "true");
    ret |= iniparse_set(config, "core", "bare", "false");
    if (ret != 0) {
      iniparse_close(config);
      goto rollback;
    }

    if (iniparse_write(config) == -1) {
      iniparse_close(config);
      goto rollback;
    }

    iniparse_close(config);
  }
  return 0;

rollback:
  rollback(repo);
  return -1;
}