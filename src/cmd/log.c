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
#include "obj/obj.h"
#include "repo.h"

int cmd_log(int argc, char **argv)
{
  int first_parent = 0;
  int oneline = 0;
  int n = -1; /* unlimited, ignore negative values */
  const char *head = "HEAD";

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
    die("%s", ctx.errstr);

  if (argparse_parse(&ctx, argc, argv) == -1)
    die("%s", ctx.errstr);

  if (argparse_getremargc(&ctx) > 0)
    head = argparse_getremargv(&ctx)[0];

  struct repo *repo = repo_cwd();
  if (!repo)
    die("not inside a qgit repository");

  unsigned char sha1[SHA1_DIGEST_LENGTH];
  if (obj_resolve(repo, head, sha1) == -1)
    die("ambiguous arguments '%s' unknown reference", head);

  struct obj *obj = obj_open_sha1(repo, sha1);
  if (!obj)
    die_errno();

  if (obj->type != OBJ_COMMIT)
    die("%s not a commit", head);

  if (n <= -1)
    n = -1; /* unlimited */

  struct commit_iter iter;
  commit_style_t style = oneline ? COMMIT_STYLE_ONELINE : COMMIT_STYLE_DEFAULT;
  commit_walk_type_t type = first_parent ? COMMIT_WALK_FIRST : COMMIT_WALK_ALL;

  if (commit_iter_init(&iter, obj, repo, type) == -1)
    die_errno();

  struct obj *cur;
  while ((cur = commit_iter_get(&iter)) && (n == -1 || n-- > 0)) {
    if (commit_fprintf_style(stdout, cur, style) == -1)
      die_errno();
    int ret = commit_iter_inc(&iter);
    if (ret == -1)
      die_errno();
    if (ret == 1)
      break;
  }

  commit_iter_fini(&iter);
  repo_free(repo);
  argparse_fini(&ctx);
  return 0;
}