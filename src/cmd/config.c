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
#include "cmd.h"
#include "error.h"
#include "iniparse.h"
#include "objs/repo.h"

#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int cmd_config(int argc, char **argv)
{
  bool global = false;
  bool local = false;
  bool list = false;

  struct argparse ctx;
  struct argparse_opt opts[] = {
      OPT_HELP(),
      OPT_BOOL(0, "global", "use global config file", &global),
      OPT_BOOL(0, "local", "use local config file", &local),
      OPT_BOOL('l', "list", "list all options", &list),
      OPT_END(),
  };
  struct argparse_desc desc = {
      .prog = "qgit",
      .desc = "Get and set repository or global options",
      .usage = "qgit config [options]",
      .epilog = "See 'qgit config --help' for more information.",
  };

  argparse_init(&ctx, opts, &desc);
  if (argparse_parse(&ctx, argc, argv) == -1)
    error("qgit: %s\n", argparse_strerror(&ctx));

  if (list) {
    if (local) {
      char path[PATH_MAX];
      if (!getcwd(path, PATH_MAX))
        fatal();
      struct repo *repo = repo_open(path);
      if (!repo || !repo->config)
        error("qgit: --local can only be used inside a qgit repository\n");
      iniparse_fprint(repo->config, stdout);
      repo_close(repo);
    } else if (global) {
      char path[PATH_MAX];
      char *home = getenv("HOME");
      snprintf(path, PATH_MAX, "%s/.qgitconfig", home);
      struct iniFILE *config = iniparse_open(path);
      if (config && iniparse_parse(config) != -1)
        iniparse_fprint(config, stdout);
      iniparse_close(config);
    } else {
      char path[PATH_MAX];
      char *home = getenv("HOME");
      snprintf(path, PATH_MAX, "%s/.qgitconfig", home);
      struct iniFILE *config = iniparse_open(path);
      if (config && iniparse_parse(config) != -1) {
        iniparse_fprint(config, stdout);
        iniparse_close(config);
      }
      *path = '\0';

      getcwd(path, PATH_MAX);
      struct repo *repo = repo_open(path);
      if (repo && repo->config) {
        iniparse_fprint(repo->config, stdout);
        repo_close(repo);
      }
    }
  } else {
    error("qgit: no action specified\n");
  }
  argparse_fini(&ctx);
  return 0;
}