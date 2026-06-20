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
#include <fcntl.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#include "argparse.h"
#include "die.h"

int cmd_hash_object(int argc, char **argv)
{
  int w = 0;
  const char *type = "blob";

  struct argparse ctx;
  struct argparse_opt opts[] = {
      OPT_HELP(),
      OPT_STR('t', "type", "specify the type of the object", &type,
              OPT_REQUIRED),
      OPT_BOOL('w', "write", "write the object to the repository", &w),
      OPT_END(),
  };

  static const char *usages[] = {
      "qgit hash-object [-t <type>] [-w | --write] <file>",
  };

  struct argparse_desc desc = {
      .prog = "qgit hash-object",
      .desc = "Compute object ID and optionally create a blob from a file",
      .usages = usages,
      .nusages = sizeof(usages) / sizeof(usages[0]),
  };

  if (argparse_init(&ctx, opts, &desc) == -1)
    die("%s", argparse_strerror(&ctx));
  if (argparse_parse(&ctx, argc, argv) == -1)
    die("%s", argparse_strerror(&ctx));

  argparse_fini(&ctx);
  return 0;
}