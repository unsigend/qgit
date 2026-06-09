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
#include <string.h>
#include <unistd.h>

#include "argparse.h"
#include "die.h"
#include "fs.h"
#include "obj/obj.h"
#include "repo.h"
#include "sha1.h"

static obj_type_t obj_type_from_str(const char *str)
{
  if (strcmp(str, "blob") == 0)
    return OBJ_BLOB;
  else if (strcmp(str, "commit") == 0)
    return OBJ_COMMIT;
  else if (strcmp(str, "tree") == 0)
    return OBJ_TREE;
  else if (strcmp(str, "tag") == 0)
    return OBJ_TAG;
  else
    return OBJ_NONE;
}

int cmd_hash_object(int argc, char **argv)
{

  const char *type = "blob";
  const char *file = NULL;
  bool w = false;

  struct argparse ctx;
  struct argparse_opt opts[] = {
      OPT_HELP(),
      OPT_STR('t', "type", "specify the type of the object", &type,
              OPT_REQUIRED),
      OPT_BOOL('w', "write", "write the object to the repository", &w),
      OPT_END(),
  };

  struct argparse_desc desc = {
      .prog = "qgit hash-object",
      .desc = "Compute object ID and optionally create a blob from a file",
      .usage = "qgit hash-object [options] <file>",
  };

  if (argparse_init(&ctx, opts, &desc) == -1)
    die_errno();

  if (argparse_parse(&ctx, argc, argv) == -1)
    die_errno();

  if (argparse_getremargc(&ctx) > 0)
    file = argparse_getremargv(&ctx)[0];

  if (!file)
    die("no file specified");

  obj_type_t obj_type = obj_type_from_str(type);
  if (obj_type == OBJ_NONE)
    die("invalid object type: %s", type);

  char path[PATH_MAX];

  if (fabspath(file, path) == -1)
    die_errno();

  struct obj *obj = obj_open_file(path, obj_type);
  if (!obj)
    die_errno();
  if (obj_sha1(obj) == -1)
    die_errno();

  unsigned char hex[SHA1_HEX_LENGTH];
  if (sha1_to_hex(obj->sha1, hex) == -1)
    die_errno();
  if (w) {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
      die_errno();
    struct repo *repo = repo_find(cwd);
    if (!repo)
      die("not inside a qgit repository");
    if (obj_write(repo, obj) == -1)
      die_errno();
    repo_free(repo);
  }
  printf("%s\n", hex);
  obj_close(obj);

  argparse_fini(&ctx);
  return 0;
}