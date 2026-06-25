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
#include "obj/object.h"
#include "obj/tree.h"
#include "ref.h"
#include "repo.h"

int cmd_ls_tree(int argc, char **argv)
{
  int r = 0;
  int t = 0;
  const char *name;

  struct argparse ctx;
  struct argparse_opt opts[] = {
      OPT_HELP(),
      OPT_BOOL('r', NULL, "Recurse into sub-tree", &r),
      OPT_BOOL('t', NULL, "Show tree entries even when going to recurse them",
               &t),
      OPT_END(),
  };

  static const char *usages[] = {
      "qgit ls-tree [-r] [<tree-ish>]",
  };

  struct argparse_desc desc = {
      .prog = "qgit ls-tree",
      .desc = "List the contents of a tree object",
      .usages = usages,
      .nusages = sizeof(usages) / sizeof(usages[0]),
  };

  if (argparse_init(&ctx, opts, &desc) == -1)
    die("%s", argparse_strerror(&ctx));
  if (argparse_parse(&ctx, argc, argv) == -1)
    die("%s", argparse_strerror(&ctx));

  if (argparse_getremargc(&ctx) < 1)
    die("missing <tree-ish>");
  name = argparse_getremargv(&ctx)[0];

  struct repo *repo = NULL;
  struct object *obj = NULL;
  enum tree_print_style style = TREE_PRINT_STYLE_DEFAULT;

  if (!((repo = repo_findcwd())))
    die_errno();

  if (!((obj = obj_find(repo, name, OBJ_TREE))))
    die_errno();

  if (obj_parse(obj) == -1)
    die_errno();

  if (r) {
    if (t)
      style = TREE_PRINT_STYLE_SHOW_TREE;
    else
      style = TREE_PRINT_STYLE_RECURSE;
  }

  if (tree_fprintf_style(obj, stdout, repo, style) == -1)
    die_errno();

  obj_close(obj);
  repo_close(repo);
  argparse_fini(&ctx);
  return 0;
}