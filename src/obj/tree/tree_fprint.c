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

#include "obj/obj.h"
#include "obj/tree.h"

int tree_fprintf(FILE *stream, struct obj *obj)
{
  if (!stream || !obj) {
    errno = EINVAL;
    return -1;
  }

  for (size_t i = 0; i < vec_size(&obj->tree.entries); i++)
    if (tree_entry_fprintf(stream, vec_at(&obj->tree.entries, i), NULL) == -1)
      return -1;
  return 0;
}
