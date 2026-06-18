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

#include "fs.h"
#include "iniparse.h"
#include "repo.h"

static mode_t dirmode = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;

#define ROLLBACK(path) rmdirr(path)
static const char *dirs[] = {"objects", "refs/heads", "refs/tags"};

struct repo *repo_create(const char *path, const char *branch)
{
  if (!path || !branch || !*branch)
    return NULL;

  char buf[PATH_MAX];
  struct repo *repo;
  int created = 0;
  FILE *fp = NULL;
  struct iniFILE *inifp = NULL;

  repo = calloc(1, sizeof(struct repo));
  if (!repo)
    return NULL;

  if (!(repo->worktree = strdup(path))) {
    repo_close(repo);
    return NULL;
  }

  if (snprintf(buf, PATH_MAX, "%s/.qgit", path) >= PATH_MAX) {
    errno = ENAMETOOLONG;
    repo_close(repo);
    return NULL;
  }

  if (!(repo->qgitdir = strdup(buf))) {
    repo_close(repo);
    return NULL;
  }

  if (!dir_exists(repo->qgitdir)) {
    if (path_exists(repo->qgitdir)) {
      errno = ENOTDIR;
      repo_close(repo);
      return NULL;
    }
    if (mkdirp(repo->qgitdir, dirmode) == -1) {
      ROLLBACK(repo->qgitdir);
      repo_close(repo);
      return NULL;
    } else
      created = 1;
  }

  for (size_t i = 0; i < sizeof(dirs) / sizeof(dirs[0]); i++) {
    if (snprintf(buf, PATH_MAX, "%s/%s", repo->qgitdir, dirs[i]) >= PATH_MAX) {
      errno = ENAMETOOLONG;
      if (created)
        ROLLBACK(repo->qgitdir);
      repo_close(repo);
      return NULL;
    }

    if (!dir_exists(buf)) {
      if (mkdirp(buf, dirmode) == -1) {
        if (created)
          ROLLBACK(repo->qgitdir);
        repo_close(repo);
        return NULL;
      }
    }
  }

  /* HEAD */
  if (snprintf(buf, PATH_MAX, "%s/HEAD", repo->qgitdir) >= PATH_MAX) {
    errno = ENAMETOOLONG;
    if (created)
      ROLLBACK(repo->qgitdir);
    repo_close(repo);
    return NULL;
  }

  if (!file_exists(buf)) {
    if (!(fp = fopen(buf, "w"))) {
      if (created)
        ROLLBACK(repo->qgitdir);
      repo_close(repo);
      return NULL;
    }

    if (fprintf(fp, "ref: refs/heads/%s\n", branch) < 0) {
      fclose(fp);
      if (created)
        ROLLBACK(repo->qgitdir);
      repo_close(repo);
      return NULL;
    }
    fclose(fp);
  }

  /* description */
  if (snprintf(buf, PATH_MAX, "%s/description", repo->qgitdir) >= PATH_MAX) {
    errno = ENAMETOOLONG;
    if (created)
      ROLLBACK(repo->qgitdir);
    repo_close(repo);
    return NULL;
  }

  if (!file_exists(buf)) {
    if (!(fp = fopen(buf, "w"))) {
      if (created)
        ROLLBACK(repo->qgitdir);
      repo_close(repo);
      return NULL;
    }

    if (fprintf(fp, "Unnamed repository; edit this file 'description' to name "
                    "the repository.\n") < 0) {
      fclose(fp);
      if (created)
        ROLLBACK(repo->qgitdir);
      repo_close(repo);
      return NULL;
    }
    fclose(fp);
  }

  /* config */
  if (snprintf(buf, PATH_MAX, "%s/config", repo->qgitdir) >= PATH_MAX) {
    errno = ENAMETOOLONG;
    if (created)
      ROLLBACK(repo->qgitdir);
    repo_close(repo);
    return NULL;
  }
  if (!file_exists(buf)) {
    if (!(inifp = iniparse_create(buf))) {
      if (created)
        ROLLBACK(repo->qgitdir);
      repo_close(repo);
      return NULL;
    }

    if (iniparse_set(inifp, "core", "repositoryformatversion", "0") == -1 ||
        iniparse_set(inifp, "core", "filemode", "false") == -1 ||
        iniparse_set(inifp, "core", "bare", "false") == -1) {
      iniparse_close(inifp);
      if (created)
        ROLLBACK(repo->qgitdir);
      repo_close(repo);
      return NULL;
    }

    if (iniparse_write(inifp) == -1) {
      iniparse_close(inifp);
      if (created)
        ROLLBACK(repo->qgitdir);
      repo_close(repo);
      return NULL;
    }
    iniparse_close(inifp);
  }

  return repo;
}