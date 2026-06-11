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
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "fs.h"
#include "iniparse.h"
#include "repo.h"

struct repo *repo_init(const char *path)
{
  if (!path) {
    errno = EINVAL;
    return NULL;
  }
  struct repo *repo = malloc(sizeof(*repo));
  if (!repo)
    return NULL;
  memset(repo, 0, sizeof(*repo));

  if (snprintf(repo->worktree, PATH_MAX, "%s", path) >= PATH_MAX) {
    repo_free(repo);
    errno = ENAMETOOLONG;
    return NULL;
  }

  if (snprintf(repo->gitdir, PATH_MAX, "%s/.qgit", repo->worktree) >=
      PATH_MAX) {
    repo_free(repo);
    errno = ENAMETOOLONG;
    return NULL;
  }

  return repo;
}

void repo_free(struct repo *repo)
{
  if (!repo)
    return;
  free(repo);
}

static const char *dirs[] = {"objects", "refs/heads", "refs/tags"};

static void rollback(const char *path)
{
  if (path_exists(path)) {
    rmdirr(path);
  }
}

int repo_create(struct repo *repo, const char *branch)
{
  if (!repo || !branch || !*branch) {
    errno = EINVAL;
    return -1;
  }

  const mode_t mode = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
  char buf[PATH_MAX];
  FILE *fp = NULL;

  /* Always assume the .qgit dir is under the working tree for the repository
     format design. */
  if (!dir_exists(repo->gitdir)) {
    if (mkdirp(repo->gitdir, mode) == -1) {
      rollback(repo->gitdir);
      return -1;
    }
  }

  for (size_t i = 0; i < sizeof(dirs) / sizeof(*dirs); i++) {
    if (snprintf(buf, sizeof(buf), "%s/%s", repo->gitdir, dirs[i]) >=
        PATH_MAX) {
      rollback(repo->gitdir);
      errno = ENAMETOOLONG;
      return -1;
    }
    if (!dir_exists(buf)) {
      if (mkdirp(buf, mode) == -1) {
        rollback(repo->gitdir);
        return -1;
      }
    }
  }

  if (snprintf(buf, sizeof(buf), "%s/HEAD", repo->gitdir) >= PATH_MAX) {
    rollback(repo->gitdir);
    errno = ENAMETOOLONG;
    return -1;
  }

  if (!file_exists(buf)) {
    if ((fp = fopen(buf, "w")) == NULL) {
      rollback(repo->gitdir);
      return -1;
    }

    if (fprintf(fp, "ref: refs/heads/%s\n", branch) < 0) {
      fclose(fp);
      rollback(repo->gitdir);
      return -1;
    }

    fclose(fp);
    fp = NULL;
  }

  if (snprintf(buf, sizeof(buf), "%s/description", repo->gitdir) >= PATH_MAX) {
    rollback(repo->gitdir);
    errno = ENAMETOOLONG;
    return -1;
  }

  if (!file_exists(buf)) {
    if ((fp = fopen(buf, "w")) == NULL) {
      rollback(repo->gitdir);
      return -1;
    }

    if (fprintf(fp, "Unnamed repository; edit this file 'description' to name "
                    "the repository.\n") < 0) {
      fclose(fp);
      rollback(repo->gitdir);
      return -1;
    }

    fclose(fp);
    fp = NULL;
  }

  if (snprintf(buf, sizeof(buf), "%s/config", repo->gitdir) >= PATH_MAX) {
    rollback(repo->gitdir);
    errno = ENAMETOOLONG;
    return -1;
  }

  if (!file_exists(buf)) {
    struct iniFILE *inifp = iniparse_create(buf);
    if (!inifp) {
      rollback(repo->gitdir);
      return -1;
    }

    if (iniparse_set(inifp, "core", "repositoryformatversion", "0") == -1 ||
        iniparse_set(inifp, "core", "filemode", "false") == -1 ||
        iniparse_set(inifp, "core", "bare", "false") == -1) {
      iniparse_close(inifp);
      rollback(repo->gitdir);
      return -1;
    }

    if (iniparse_write(inifp) == -1) {
      iniparse_close(inifp);
      rollback(repo->gitdir);
      return -1;
    }
    iniparse_close(inifp);
  }

  return 0;
}

struct repo *repo_find(const char *path)
{
  if (!path)
    return NULL;

  char buf[PATH_MAX];
  char qgit[PATH_MAX];

  if (snprintf(buf, sizeof(buf), "%s", path) >= PATH_MAX) {
    errno = ENAMETOOLONG;
    return NULL;
  }

  while (1) {
    if (snprintf(qgit, sizeof(qgit), "%s/.qgit", buf) >= PATH_MAX) {
      errno = ENAMETOOLONG;
      return NULL;
    }
    if (dir_exists(qgit))
      break;
    char *slash = strrchr(buf, '/');
    *slash = '\0';
    if (slash == buf) {
      errno = ENOENT;
      return NULL;
    }
  }

  struct repo *repo = repo_init(buf);
  if (!repo)
    return NULL;

  return repo;
}

struct repo *repo_cwd(void)
{
  char cwd[PATH_MAX];
  if (getcwd(cwd, sizeof(cwd)) == NULL)
    return NULL;
  return repo_find(cwd);
}