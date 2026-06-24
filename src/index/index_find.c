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

#include <string.h>

#include "index.h"

/* linear search O(n) */
struct index_entry *index_find(struct index *index, const char *path,
                               uint16_t stage)
{
  if (!index || !path)
    return NULL;

  struct index_entry *entry;
  struct vector_iter iter;

  if (vec_iter_init(&iter, &index->entries) == -1)
    return NULL;

  while ((entry = vec_iter_get(&iter))) {
    if (entry->stage == stage && strcmp(entry->path, path) == 0)
      return entry;
    vec_iter_inc(&iter);
  }

  return NULL;
}