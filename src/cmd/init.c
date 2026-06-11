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
#include <stdio.h>

#include "argparse.h"
#include "die.h"
#include "fs.h"
#include "repo.h"

int cmd_init(int argc, char **argv)
{
  const char *bname = "main";
  const char *path = ".";
  int quiet = 0;

  struct argparse ctx;
  struct argparse_opt opts[] = {
      OPT_HELP(),
      OPT_STR('b', "initial-branch", "override the name of initial branch",
              &bname, OPT_REQUIRED),
      OPT_BOOL('q', "quiet", "suppress non-error messages", &quiet), OPT_END()};

  static const char *usages[] = {
      "qgit init [-q | --quiet] [-b <branch-name> | "
      "--initial-branch=<branch-name>] [<directory>]",
  };

  struct argparse_desc desc = {
      .prog = "qgit init",
      .desc = "Create an empty Git repository or reinitialize an existing one",
      .usages = usages,
      .nusages = sizeof(usages) / sizeof(usages[0]),
      .epilog = "See 'qgit init --help' for more information.",
  };

  if (argparse_init(&ctx, opts, &desc) == -1)
    die("%s", ctx.errstr);

  if (argparse_parse(&ctx, argc, argv) == -1)
    die("%s", ctx.errstr);

  if (argparse_getremargc(&ctx) > 0)
    path = argparse_getremargv(&ctx)[0];

  char abspath[PATH_MAX];
  if (fabspath(path, abspath) == -1)
    die_errno();

  struct repo *repo = repo_init(abspath);
  if (!repo)
    die_errno();

  int reinit = 0;
  if (dir_exists(repo->gitdir))
    reinit = 1;

  if (repo_create(repo, bname) == -1) {
    if (errno == EINVAL)
      die("invalid branch name: %s", bname);
    die_errno();
  }

  if (!quiet) {
    if (reinit)
      printf("Reinitialized existing qgit repository in %s\n", abspath);
    else
      printf("Initialized empty qgit repository in %s\n", abspath);
  }

  repo_free(repo);
  argparse_fini(&ctx);
  return 0;
}