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

#include "argparse.h"
#include "cmd.h"
#include "error.h"
#include "objs/object.h"

int cmd_hash_object(int argc, char **argv)
{
  bool write = false;
  const char *type = NULL;

  struct argparse ctx;
  struct argparse_opt opts[] = {
      OPT_HELP(),
      OPT_BOOL('w', NULL, "write the object to the repository", &write),
      OPT_STR('t', NULL, "object type", &type, OPT_REQUIRED),
      OPT_END(),
  };
  struct argparse_desc desc = {
      .prog = "qgit",
      .desc = "Create an object from a file or stdin",
      .usage = "qgit hash-object [options] <file>",
      .epilog = "See 'qgit hash-object --help' for more information.",
  };

  argparse_init(&ctx, opts, &desc);
  if (argparse_parse(&ctx, argc, argv) == -1)
    error("qgit: %s\n", argparse_strerror(&ctx));

  const char *filename = NULL;
  if (argparse_getremargc(&ctx) >= 1)
    filename = argparse_getremargv(&ctx)[0];
  else {
    argparse_fini(&ctx);
    error("qgit: a file is required\n");
  }

  struct object *obj;
  if (type) {
    if (strcmp(type, "commit") == 0)
      obj = obj_init(OBJ_COMMIT, filename);
    else if (strcmp(type, "tree") == 0)
      obj = obj_init(OBJ_TREE, filename);
    else if (strcmp(type, "blob") == 0)
      obj = obj_init(OBJ_BLOB, filename);
    else if (strcmp(type, "tag") == 0)
      obj = obj_init(OBJ_TAG, filename);
    else {
      argparse_fini(&ctx);
      error("qgit: invalid object type '%s'\n", type);
    }
  } else
    /* default to blob */
    obj = obj_init(OBJ_BLOB, filename);

  if (!obj) {
    argparse_fini(&ctx);
    fatal();
  }

  if (obj_hash(obj) == -1) {
    obj_free(obj);
    argparse_fini(&ctx);
    fatal();
  }
  printf("%s\n", obj->sha1);

  if (write) {
    struct repo *repo = repo_find(".");
    if (!repo) {
      obj_free(obj);
      argparse_fini(&ctx);
      fatal();
    }
    if (obj_write(repo, obj) == -1) {
      obj_free(obj);
      repo_close(repo);
      argparse_fini(&ctx);
      fatal();
    }
    repo_close(repo);
  }

  obj_free(obj);
  argparse_fini(&ctx);

  return 0;
}