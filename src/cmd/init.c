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

#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "argparse.h"
#include "error.h"
#include "file.h"
#include "repo.h"
#include "string.h"

int cmd_init(int argc, char **argv)
{
  const char *branch = "main";
  bool quiet = false;
  const char *path = ".";

  struct argparse parse;
  struct argparse_opt opts[] = {
      OPT_HELP(),
      OPT_STR('b', "initial-branch",
              "Use <value> for the initial branch in the newly created "
              "repository",
              &branch, OPT_OPTIONAL),
      OPT_BOOL('q', "quiet", "Suppress progress printing", &quiet),
      OPT_END(),
  };

  struct argparse_desc desc = {
      .prog = "qgit init",
      .desc = "Create an empty qgit repository or reinitialize an existing one",
      .usage = "qgit init [options] [path]",
      .epilog = "See 'qgit init --help' for more information.",
  };

  if (argparse_init(&parse, opts, &desc) == -1)
    die("%s", argparse_strerror(&parse));

  if (argparse_parse(&parse, argc, argv) == -1)
    die("%s", argparse_strerror(&parse));

  if (argparse_getremargc(&parse) > 0)
    path = argparse_getremargv(&parse)[0];

  char abspath[PATH_MAX];
  // TODO: fix the realpath path fall back check
  if (!realpath(path, abspath)) {
    if (strlen(path) >= PATH_MAX)
      die("Path too long");
    strcpy(abspath, path);
  }

  struct repo *repo = repo_init(abspath);
  bool reinit = false;
  if (!repo)
    die_errno();

  if (existdir(repo->gitdir))
    reinit = true;

  if (repo_create(repo, branch) == -1)
    die_errno();

  if (!quiet) {
    if (reinit)
      printf("Reinitialized existing qgit repository in %s\n", repo->worktree);
    else
      printf("Initialized empty qgit repository in %s\n", repo->worktree);
  }

  repo_free(repo);
  argparse_fini(&parse);
  return EXIT_SUCCESS;
}