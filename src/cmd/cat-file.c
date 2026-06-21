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
#include "ref.h"
#include "repo.h"

static void mutex_check(int p, int t, int s)
{
  if (p && t)
    die("cannot use -p and -t together");
  if (p && s)
    die("cannot use -p and -s together");
  if (t && s)
    die("cannot use -t and -s together");
}

int cmd_cat_file(int argc, char **argv)
{
  int p, t, s;
  p = t = s = 0;

  struct argparse ctx;
  struct argparse_opt opts[] = {
      OPT_HELP(),
      OPT_BOOL('p', NULL, "print the object contents in a pretty format", &p),
      OPT_BOOL('t', NULL, "show the object type", &t),
      OPT_BOOL('s', NULL, "show the object size", &s),
      OPT_END(),
  };

  static const char *usages[] = {
      "qgit cat-file (-p | -t | -s) <object>",
      "qgit cat-file <type> <object>",
  };

  struct argparse_desc desc = {
      .prog = "qgit cat-file",
      .desc = "Provide contents or details of repository objects",
      .usages = usages,
      .nusages = sizeof(usages) / sizeof(usages[0]),
  };

  if (argparse_init(&ctx, opts, &desc) == -1)
    die("%s", argparse_strerror(&ctx));
  if (argparse_parse(&ctx, argc, argv) == -1)
    die("%s", argparse_strerror(&ctx));

  struct obj *obj = NULL;
  struct repo *repo = NULL;
  const char *name = NULL;

  mutex_check(p, t, s);
  int any = p || t || s;

  repo = repo_findcwd();
  if (!repo)
    die_errno();

  if (any) /* auto mode */
  {
    if (argparse_getremargc(&ctx) < 1)
      die("auto mode requires <object>");
    name = argparse_getremargv(&ctx)[0];

    if (!((obj = obj_find(repo, name))))
      die_errno();

    if (p) {
      if (obj_parse(obj) == -1)
        die_errno();
      if (obj_fprintf(obj, stdout) == -1)
        die_errno();
    } else if (t) {
      const char *otype = obj_type_to_str(obj->type);
      if (otype == NULL)
        die_errno();
      printf("%s\n", otype);
    } else if (s) {
      printf("%zu\n", obj->payloadsz);
    }

  } else /* raw mode */
  {
    if (argparse_getremargc(&ctx) < 2)
      die("raw mode requires <type> <object>");
    name = argparse_getremargv(&ctx)[1];
    const char *expect = argparse_getremargv(&ctx)[0];

    if (obj_type_from_str(expect) == OBJ_NONE)
      die_errno();

    if (!((obj = obj_find(repo, name))))
      die_errno();

    if (obj->type != obj_type_from_str(expect))
      die("expect object type '%s', but got '%s'", expect,
          obj_type_to_str(obj->type));
    if (fwrite(obj->payload, 1, obj->payloadsz, stdout) != obj->payloadsz)
      die_errno();
  }

  obj_close(obj);
  repo_close(repo);
  argparse_fini(&ctx);
  return 0;
}