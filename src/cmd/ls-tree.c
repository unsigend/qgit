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
#include "obj/obj.h"

int cmd_ls_tree(int argc, char **argv)
{
  int r = 0;
  int t = 0;
  const char *name = "HEAD";
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
    die("%s", ctx.errstr);

  if (argparse_parse(&ctx, argc, argv) == -1)
    die("%s", ctx.errstr);

  if (argparse_getremargc(&ctx) > 0)
    name = argparse_getremargv(&ctx)[0];

  struct repo *repo = repo_cwd();
  if (!repo)
    die("not inside a qgit repository");

  unsigned char sha1[SHA1_DIGEST_LENGTH];
  if (obj_resolve(repo, name, sha1) == -1)
    die_errno();

  struct obj *obj = obj_open_sha1(repo, sha1);
  if (!obj)
    die_errno();

  if (obj_parse(obj) == -1)
    die_errno();

  if (obj->type == OBJ_TREE) {
    if (r) {
      if (tree_fprintf_r(stdout, obj, repo, t) == -1)
        die_errno();
    } else {
      if (obj_fprintf(stdout, obj) == -1)
        die_errno();
    }
  } else if (obj->type == OBJ_COMMIT) {
    struct obj *tree_obj = obj_open_sha1(repo, obj->commit.tree);
    if (!tree_obj)
      die_errno();

    if (obj_parse(tree_obj) == -1)
      die_errno();

    if (r) {
      if (tree_fprintf_r(stdout, tree_obj, repo, t) == -1)
        die_errno();
    } else {
      if (obj_fprintf(stdout, tree_obj) == -1)
        die_errno();
    }
    obj_close(tree_obj);
  } else
    die("not a tree object");

  obj_close(obj);
  repo_free(repo);
  argparse_fini(&ctx);
  return 0;
}