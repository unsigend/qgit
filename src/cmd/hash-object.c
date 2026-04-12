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

#include "argparse.h"
#include "cmd.h"
#include "error.h"
#include "objs/object.h"

int cmd_hash_object(int argc, char **argv)
{
  bool write = false;

  struct argparse ctx;
  struct argparse_opt opts[] = {
      OPT_HELP(),
      OPT_BOOL('w', NULL, "Write the object to the repository", &write),
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

  /* Current implementation only supports blob objects */
  struct object *obj = object_open(OBJ_BLOB, filename);
  if (!obj) {
    argparse_fini(&ctx);
    fatal();
  }

  if (object_hash(obj) == -1) {
    object_free(obj);
    argparse_fini(&ctx);
    fatal();
  }
  printf("%s\n", obj->sha1);

  if (write) {
    struct repo *repo = repo_find(".");
    if (!repo) {
      object_free(obj);
      argparse_fini(&ctx);
      fatal();
    }
    if (object_write(repo, obj) == -1) {
      object_free(obj);
      repo_close(repo);
      argparse_fini(&ctx);
      fatal();
    }
    repo_close(repo);
  }

  object_free(obj);
  argparse_fini(&ctx);

  return 0;
}