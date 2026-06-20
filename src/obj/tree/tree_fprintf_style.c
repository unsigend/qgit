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

#include "obj/object.h"
#include "obj/tree.h"

struct tree_print_ctx {
  FILE *fp;
  enum tree_print_style style;
};

static int print_tree(struct tree_entry *entry, const char *prefix, void *arg)
{
  struct tree_print_ctx *ctx = (struct tree_print_ctx *)arg;
  if (!ctx || !ctx->fp)
    return -1;

  if (S_ISDIR(entry->mode)) {
    if (ctx->style == TREE_PRINT_STYLE_SHOW_TREE) {
      if (tree_entry_fprintf(entry, prefix, ctx->fp) == -1)
        return -1;
    }
    return 0;
  }

  if (tree_entry_fprintf(entry, prefix, ctx->fp) == -1)
    return -1;

  return 0;
}

int tree_fprintf_style(struct obj *obj, FILE *fp, struct repo *repo,
                       enum tree_print_style style)
{
  if (!obj || !fp || obj->type != OBJ_TREE)
    return -1;

  struct tree_print_ctx ctx = {
      .fp = fp,
      .style = style,
  };

  if (style == TREE_PRINT_STYLE_DEFAULT) {
    return tree_fprintf(obj, fp);
  } else {
    return tree_foreach(obj, repo, print_tree, &ctx);
  }
}