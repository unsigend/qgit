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

#include <stdlib.h>

#include "index.h"

int index_add(struct index *index, struct index_entry *entry)
{
  if (!index || !entry || !entry->path)
    return -1;

  struct index_entry *existing = index_find(index, entry->path, entry->stage);
  if (existing) {
    if (entry == existing)
      return 0;
    free(existing->path);
    *existing = *entry; /* shadow copy transfer ownership of path */
    return 0;
  }

  if (vec_pushback(&index->entries, entry) == -1)
    return -1;

  return 0;
}