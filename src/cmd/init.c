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

#include <argparse.h>
#include <cmd.h>
#include <repo.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <util.h>

int cmd_init(int argc, char **argv)
{
  bool bare = false;
  bool quiet = false;
  char *bname = NULL;
  char *path = NULL;

  struct argparse ctx;
  struct argparse_opt opts[] = {
      OPT_HELP(),
      OPT_BOOL(0, "bare", "Create a bare repository", &bare),
      OPT_BOOL('q', "quiet", "Only print error and warning messages", &quiet),
      OPT_STR('b', "initial-branch", "Specify the name of the initial branch",
              &bname, OPT_REQUIRED),
      OPT_END(),
  };
  struct argparse_desc desc = {
      .prog = "qgit",
      .desc = "Initialize a new repository",
      .usage = "qgit init [options]",
      .epilog = "See 'qgit init --help' for more information.",
  };

  argparse_init(&ctx, opts, &desc);
  if (argparse_parse(&ctx, argc, argv) == -1) {
    error("qgit: %s\n", argparse_strerror(&ctx));
    return -1;
  }

  if (!bname)
    bname = "main"; /* TODO: get this from config file, init.defaultBranch
                       configuration */

  char **remargv = argparse_getremargv(&ctx);
  if (remargv)
    path = remargv[0];
  else
    path = ".";

  struct repo repo;

  (void)path;
  (void)repo;

  argparse_fini(&ctx);
  return 0;
}