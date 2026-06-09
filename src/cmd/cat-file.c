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

#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#include "argparse.h"
#include "die.h"
#include "obj/obj.h"
#include "repo.h"
#include "sha1.h"

static void printcheck(bool p, bool s, bool t)
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
  bool p, t, s;
  p = t = s = false;
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

  struct argparse_desc desc = {
      .prog = "qgit cat-file",
      .desc = "Provide contents or details of repository objects",
      .usage = "qgit cat-file [options] <object>",
  };

  if (argparse_init(&ctx, opts, &desc) == -1)
    die_errno();

  if (argparse_parse(&ctx, argc, argv) == -1)
    die_errno();

  printcheck(p, s, t);

  bool has_flag = p || s || t;
  if (has_flag) {
    if (argparse_getremargc(&ctx) < 1)
      die("requires an <object> argument");
    name = argparse_getremargv(&ctx)[0];
  } else {
    if (argparse_getremargc(&ctx) < 2)
      die("requires 2 arguments in <type> <object>");
    type = argparse_getremargv(&ctx)[0];
    name = argparse_getremargv(&ctx)[1];
    if (obj_type_from_str(type) == OBJ_NONE)
      die("invalid object type: %s", type);
  }

  char cwd[PATH_MAX];
  if (getcwd(cwd, sizeof(cwd)) == NULL)
    die_errno();

  struct repo *repo = repo_find(cwd);
  if (!repo)
    die("not inside a qgit repository");

  unsigned char sha1[SHA1_DIGEST_LENGTH];
  if (obj_resolve(repo, name, sha1) == -1)
    die_errno();

  struct obj *obj = obj_open_sha1(repo, sha1);
  if (!obj)
    die_errno();

  if (p)
    fwrite(obj->payload, 1, obj->payloadsz, stdout);
  else if (s)
    printf("%zu\n", obj->payloadsz);
  else if (t) {
    const char *str = str_from_obj_type(obj->type);
    if (!str)
      die("broken object file");
    printf("%s\n", str);
  } else {
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