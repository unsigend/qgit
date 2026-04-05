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

#include "repo.h"

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define PERM                                                                   \
  (S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)

static const char *dirs[] = {
    "objects", "objects/info", "objects/pack",
    "refs",    "refs/heads",   "refs/tags",
};

int repo_init(struct repo *repo, const char *path, bool bare, bool check)
{
  if (!repo || !path)
    return -1;
  const char *qgit_dir = ".qgit";
  struct stat st;
  if (check && (stat(path, &st) == -1 || !S_ISDIR(st.st_mode)))
    return -1;
  repo->worktree = strdup(path);
  if (!repo->worktree)
    return -1;
  repo->qgit_dir = malloc(strlen(path) + strlen(qgit_dir) + 2);
  if (!repo->qgit_dir) {
    free(repo->worktree);
    return -1;
  }
  snprintf(repo->qgit_dir, strlen(path) + strlen(qgit_dir) + 2, "%s/%s", path,
           qgit_dir);

  repo->bare = bare;
  return 0;
}

int repo_create(const struct repo *repo, const char *bname)
{
  if (!repo || !bname)
    return -1;

  int re = 0; /* reinitialize */
  if (mkdir(repo->qgit_dir, PERM) == -1) {
    if (errno == EEXIST) {
      re = 1;
      goto done;
    }
    return -1;
  }

  char buf[PATH_MAX];
  const char *dirname = repo->qgit_dir;

  for (size_t i = 0; i < sizeof(dirs) / sizeof(dirs[0]); i++) {
    snprintf(buf, PATH_MAX, "%s/%s", dirname, dirs[i]);
    if (mkdir(buf, PERM) == -1)
      return -1;
  }

  snprintf(buf, PATH_MAX, "%s/description", dirname);
  FILE *fp = fopen(buf, "w");
  if (!fp)
    return -1;
  fprintf(fp, "Unnamed repository; edit this file 'description' to name the "
              "repository.\n");
  fclose(fp);

  snprintf(buf, PATH_MAX, "%s/HEAD", dirname);
  fp = fopen(buf, "w");
  if (!fp)
    return -1;
  fprintf(fp, "ref: refs/heads/%s\n", bname);
  fclose(fp);

  snprintf(buf, PATH_MAX, "%s/config", dirname);
  fp = fopen(buf, "w");
  if (!fp)
    return -1;
  fprintf(fp, "[core]\n");
  fprintf(fp, "\trepositoryformatversion = 0\n");
  fprintf(fp, "\tfilemode = true\n");
  fprintf(fp, "\tbare = %s\n", repo->bare ? "true" : "false");
  fclose(fp);

done:
  if (re)
    printf("Reinitialized existing qgit repository in %s/\n", repo->qgit_dir);
  else
    printf("Initialized empty qgit repository in %s/\n", repo->qgit_dir);

  return 0;
}

void repo_fini(struct repo *repo)
{
  if (!repo)
    return;
  if (repo->worktree)
    free(repo->worktree);
  if (repo->qgit_dir)
    free(repo->qgit_dir);
}