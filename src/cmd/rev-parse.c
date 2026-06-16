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
#include <stddef.h>
#include <stdio.h>

#include "argparse.h"
#include "die.h"
#include "obj/obj.h"
#include "repo.h"
#include "sha1.h"

int cmd_rev_parse(int argc, char **argv)
{
  struct argparse ctx;
  struct argparse_opt opts[] = {
      OPT_HELP(),
      OPT_END(),
  };

  static const char *usages[] = {
      "qgit rev-parse [options] <args>",
  };

  struct argparse_desc desc = {
      .prog = "qgit rev-parse",
      .desc = "Pick out and massage parameters",
      .usages = usages,
      .nusages = sizeof(usages) / sizeof(usages[0]),
  };

  if (argparse_init(&ctx, opts, &desc) == -1)
    die("%s", ctx.errstr);

  if (argparse_parse(&ctx, argc, argv) == -1)
    die("%s", ctx.errstr);

  struct repo *repo = repo_cwd();
  if (!repo)
    die("not inside a qgit repository");

  if (argparse_getremargc(&ctx) > 0) {
    const char *name;
    unsigned char sha1[SHA1_DIGEST_LENGTH];
    unsigned char hex[SHA1_HEX_LENGTH];

    for (size_t i = 0; i < argparse_getremargc(&ctx); i++) {
      name = argparse_getremargv(&ctx)[i];
      if (obj_resolve(repo, name, sha1) == -1) {
        if (errno == EINVAL)
          die("ambiguous arguments '%s' unknown reference", name);
        die_errno();
      }

      if (sha1_to_hex(sha1, hex) == -1)
        die_errno();

      printf("%s\n", hex);
    }
  }

  repo_free(repo);
  argparse_fini(&ctx);
  return 0;
}