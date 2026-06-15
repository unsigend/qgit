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

#include "argparse.h"
#include "die.h"

int cmd_show_ref(int argc, char **argv)
{
  int head = 0;
  int branches = 0;
  int tags = 0;
  struct argparse ctx;
  struct argparse_opt opts[] = {
      OPT_HELP(),
      OPT_BOOL(0, "head", "Show the HEAD reference", &head),
      OPT_BOOL(0, "branches", "Show all branches", &branches),
      OPT_BOOL(0, "tags", "Show all tags", &tags),
      OPT_END(),
  };

  static const char *usages[] = {
      "qgit show-ref [--head]",
  };
  struct argparse_desc desc = {
      .prog = "qgit show-ref",
      .desc = "List references in a local repository",
      .usages = usages,
      .nusages = sizeof(usages) / sizeof(usages[0]),
  };

  if (argparse_init(&ctx, opts, &desc) == -1)
    die("%s", ctx.errstr);

  if (argparse_parse(&ctx, argc, argv) == -1)
    die("%s", ctx.errstr);

  argparse_fini(&ctx);
  return 0;
}