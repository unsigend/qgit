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

#include <stdio.h>
#include <unistd.h>

#include "argparse.h"
#include "die.h"
#include "obj/obj.h"

static void mutex_check(int p, int s, int t)
{
  if (p && s)
    die("options -p and -s cannot be used together");
  if (p && t)
    die("options -p and -t cannot be used together");
  if (s && t)
    die("options -s and -t cannot be used together");
}

int cmd_cat_file(int argc, char **argv)
{
  int p, t, s;
  p = t = s = 0;
  const char *type = NULL;
  const char *name = NULL;

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
    die("%s", ctx.errstr);

  if (argparse_parse(&ctx, argc, argv) == -1)
    die("%s", ctx.errstr);

  mutex_check(p, s, t);

  int any = p || s || t;
  if (any) {
    if (argparse_getremargc(&ctx) < 1)
      die("auto mode requires an <object> argument");
    name = argparse_getremargv(&ctx)[0];
  } else {
    if (argparse_getremargc(&ctx) < 2)
      die("raw mode requires 2 arguments in <type> <object>");
    type = argparse_getremargv(&ctx)[0];
    name = argparse_getremargv(&ctx)[1];
    if (obj_type_from_str(type) == OBJ_NONE)
      die("invalid object type: %s", type);
  }

  struct repo *repo = repo_cwd();
  if (!repo)
    die("not inside a qgit repository");

  unsigned char sha1[SHA1_DIGEST_LENGTH];
  if (obj_resolve(repo, name, sha1) == -1)
    die_errno();

  struct obj *obj = obj_open_sha1(repo, sha1);
  if (!obj)
    die_errno();

  if (p) {
    if (obj_parse(obj) == -1) /* parse the object */
      die_errno();
    if (obj_fprintf(stdout, obj) == -1)
      die_errno();
  } else if (s)
    printf("%zu\n", obj->payloadsz);
  else if (t) {
    const char *str = str_from_obj_type(obj->type);
    if (!str)
      die("broken object file");
    printf("%s\n", str);
  } else { /* raw mode */
    obj_type_t expected = obj_type_from_str(type);
    if (obj->type != expected)
      die("object %s is a %s, not a %s", name, str_from_obj_type(obj->type),
          type);
    fwrite(obj->payload, 1, obj->payloadsz, stdout);
  }

  obj_close(obj);
  repo_free(repo);
  argparse_fini(&ctx);
  return 0;
}