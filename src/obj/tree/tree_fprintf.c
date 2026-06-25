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

#include "obj/object.h"
#include "obj/tree.h"

int tree_fprintf(struct object *obj, FILE *fp)
{
  if (!obj || !fp || obj->type != OBJ_TREE)
    return -1;

  struct vector_iter iter;

  if (vec_iter_init(&iter, &obj->tree.entries) == -1)
    return -1;

  while (vec_iter_get(&iter)) {
    struct tree_entry *entry = vec_iter_get(&iter);
    if (tree_entry_fprintf(entry, NULL, fp) == -1)
      return -1;
    vec_iter_inc(&iter);
  }

  return 0;
}