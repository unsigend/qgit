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
#include "cmd.h"
#include "error.h"
#include "objs/object.h"
#include "objs/repo.h"

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

static void validate_opts(bool type, bool size, bool payload)
{
  if (type && size)
    error("qgit: options '-t' and '-s' cannot be used together\n");
  if (type && payload)
    error("qgit: options '-t' and '-p' cannot be used together\n");
  if (size && payload)
    error("qgit: options '-s' and '-p' cannot be used together\n");
}

static void print_type(int type)
{
  switch (type) {
  case OBJ_COMMIT:
    puts("commit");
    break;
  case OBJ_TREE:
    puts("tree");
    break;
  case OBJ_BLOB:
    puts("blob");
    break;
  case OBJ_TAG:
    puts("tag");
    break;
  default:
    puts("unknown");
    break;
  }
}

int cmd_cat_file(int argc, char **argv)
{
  bool type = false;
  bool size = false;
  bool payload = false;

  struct argparse ctx;
  struct argparse_opt opts[] = {
      OPT_HELP(),
      OPT_BOOL('t', NULL, "Show object type", &type),
      OPT_BOOL('s', NULL, "Show object size", &size),
      OPT_BOOL('p', NULL, "Show object payload", &payload),
      OPT_END(),
  };
  struct argparse_desc desc = {
      .prog = "qgit",
      .desc = "Show object contents",
      .usage = "qgit cat-file [options] <hash>",
      .epilog = "See 'qgit cat-file --help' for more information.",
  };

  argparse_init(&ctx, opts, &desc);
  if (argparse_parse(&ctx, argc, argv) == -1)
    error("qgit: %s\n", argparse_strerror(&ctx));

  validate_opts(type, size, payload);

  const char *hash = NULL;
  if (argparse_getremargc(&ctx) >= 1)
    hash = argparse_getremargv(&ctx)[0];
  else
    error("qgit: a hash is required\n");

  struct repo *repo = repo_find(".");
  if (!repo) {
    argparse_fini(&ctx);
    error("qgit: not a qgit repository (or any of the parent directories)\n");
  }

  errno = 0;
  struct object *obj = object_read(repo, hash);
  if (!obj) {
    repo_close(repo);
    argparse_fini(&ctx);
    if (errno)
      fatal();
    else
      error("qgit: not a valid object name '%s'\n", hash);
  }

  if (type)
    print_type(obj->type);
  if (size)
    printf("%zu\n", obj->size);
  if (payload)
    fwrite(obj->payload, 1, obj->size, stdout);

  fflush(stdout);
  object_free(obj);
  repo_close(repo);
  argparse_fini(&ctx);
  return 0;
}