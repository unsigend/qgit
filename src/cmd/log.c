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
#include "obj/commit.h"
#include "obj/object.h"
#include "ref.h"
#include "repo.h"
#include "sha1.h"

#define UNLIMITED -1

int cmd_log(int argc, char **argv)
{
  int oneline = 0;
  int first_parent = 0;
  int n = UNLIMITED; /* unlimited */
  const char *name = "HEAD";

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
      .epilog = "start from HEAD by default",
  };

  if (argparse_init(&ctx, opts, &desc) == -1)
    die("%s", argparse_strerror(&ctx));
  if (argparse_parse(&ctx, argc, argv) == -1)
    die("%s", argparse_strerror(&ctx));

  struct repo *repo = NULL;
  struct obj *obj = NULL;
  struct commit_iter iter;

  if (argparse_getremargc(&ctx) > 0)
    name = argparse_getremargv(&ctx)[0];

  if (n < 0) /* ignore negative values */
    n = UNLIMITED;

  if (!(repo = repo_findcwd()))
    die_errno();

  if (!((obj = obj_find(repo, name))))
    die_errno();

  if (obj_parse_payload(obj) == -1)
    die_errno();

  if (commit_iter_init(&iter, repo, obj,
                       first_parent ? COMMIT_WALK_FPARENT : COMMIT_WALK_ALL) ==
      -1)
    die_errno();

  while ((obj = commit_iter_get(&iter)) && (n == UNLIMITED || n > 0)) {
    if (commit_fprintf_style(obj, stdout,
                             oneline ? COMMIT_PRINT_STYLE_ONELINE
                                     : COMMIT_PRINT_STYLE_DEFAULT) == -1)
      die_errno();
    int ret = commit_iter_inc(&iter);
    if (ret == -1)
      die_errno();
    if (ret == 1)
      break;
    if (n != UNLIMITED)
      n--;
  }

  commit_iter_fini(&iter);
  repo_close(repo);
  argparse_fini(&ctx);
  return 0;
}