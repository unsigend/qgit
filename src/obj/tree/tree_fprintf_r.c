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
#include <sys/stat.h>

#include "obj/obj.h"
#include "obj/tree.h"

struct tree_fprintf_arg {
  FILE *stream;
  int showtree;
};

static int print_entry(struct tree_entry *entry, const char *prefix, void *arg)
{
  struct tree_fprintf_arg *ctx = (struct tree_fprintf_arg *)arg;
  if (!ctx) {
    errno = EINVAL;
    return -1;
  }

  if (S_ISDIR(entry->mode) && !ctx->showtree)
    return 0;

  return tree_entry_fprintf(ctx->stream, entry, prefix);
}

int tree_fprintf_r(FILE *stream, struct obj *obj, struct repo *repo,
                   int showtree)
{
  if (!stream || !obj || !repo) {
    errno = EINVAL;
    return -1;
  }

  struct tree_fprintf_arg arg = {.stream = stream, .showtree = showtree};

  return tree_traverse(obj, print_entry, repo, &arg);
}