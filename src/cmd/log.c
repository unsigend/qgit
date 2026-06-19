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
#include "die.h"
#include "repo.h"
#include "sha1.h"

int cmd_log(int argc, char **argv)
{
  int oneline = 0;
  int first_parent = 0;
  int n = 0;

  struct argparse ctx;
  struct argparse_opt opts[] = {
      OPT_HELP(),
      OPT_BOOL(0, "oneline", "show commit logs in one line", &oneline),
      OPT_BOOL(0, "first-parent", "show only the first parent of each commit",
               &first_parent),
      OPT_INT('n', NULL, "limit the number of commits to show", &n,
              OPT_REQUIRED),
      OPT_END(),
  };
  static const char *usages[] = {
      "qgit log [--oneline] [--first-parent] [-n <num>] [<commit>]",
  };

  struct argparse_desc desc = {
      .prog = "qgit log",
      .desc = "Show commit logs",
      .usages = usages,
      .nusages = sizeof(usages) / sizeof(usages[0]),
  };

  if (argparse_init(&ctx, opts, &desc) == -1)
    die("%s", argparse_strerror(&ctx));
  if (argparse_parse(&ctx, argc, argv) == -1)
    die("%s", argparse_strerror(&ctx));

  struct repo *repo = NULL;
  // struct obj *obj = NULL;
  // unsigned char sha1[SHA1_DIGLEN];

  if (!(repo = repo_findcwd()))
    die_errno();

  argparse_fini(&ctx);
  return 0;
}