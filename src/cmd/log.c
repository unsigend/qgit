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

#include <stdbool.h>

int cmd_log(int argc, char **argv)
{
  bool first_parent = false;
  struct argparse ctx;
  struct argparse_opt opts[] = {
      OPT_HELP(),
      OPT_BOOL(0, "--first-parent",
               "Show commit history of the first parent only", &first_parent),
      OPT_END(),
  };
  struct argparse_desc desc = {
      .prog = "qgit",
      .desc = "Show commit logs",
      .usage = "qgit log [options]",
      .epilog = "See 'qgit log --help' for more information.",
  };

  argparse_init(&ctx, opts, &desc);
  if (argparse_parse(&ctx, argc, argv) == -1)
    error("qgit: %s\n", argparse_strerror(&ctx));

  argparse_fini(&ctx);
  return 0;
}