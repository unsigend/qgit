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

#include "argparse.h"
#include "config.h"
#include "error.h"
#include "file.h"
#include "iniparse.h"
#include "objs/repo.h"

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int cmd_init(int argc, char **argv)
{
  bool bare = false;
  bool quiet = false;
  const char *bname = NULL;
  char *path = NULL;
  struct iniFILE *gcfg = NULL;

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
  if (argparse_parse(&ctx, argc, argv) == -1)
    error("qgit: %s\n", argparse_strerror(&ctx));

  if (!bname) {
    gcfg = config_global();
    if (gcfg) {
      bname = iniparse_get(gcfg, "init", "defaultBranch");
      if (!bname)
        bname = "main";
    } else
      bname = "main";
  }

  if (argparse_getremargc(&ctx) > 0) {
    path = argparse_getremargv(&ctx)[0];
    if (mkdirp(path, PERM_DIR) == -1 && errno != EEXIST)
      fatal();
  } else
    path = ".";

  char abspath[PATH_MAX];
  if (!realpath(path, abspath))
    fatal();

  struct repo *repo = repo_create(abspath, bname, bare);
  if (!repo)
    fatal();

  if (!quiet) {
    if (repo->reinit)
      printf("Reinitialized existing qgit repository in %s/\n", repo->qgit);
    else
      printf("Initialized empty qgit repository in %s/\n", repo->qgit);
  }

  repo_close(repo);
  iniparse_close(gcfg);

  argparse_fini(&ctx);
  return 0;
}