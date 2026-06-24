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
#include <string.h>

#include "index.h"

int index_rm(struct index *index, const char *path, uint16_t stage)
{
  if (!index || !path)
    return -1;

  size_t idx;
  for (idx = 0; idx < vec_size(&index->entries); idx++) {
    struct index_entry *entry = vec_at(&index->entries, idx);
    if (entry->stage == stage && strcmp(entry->path, path) == 0) {
      if (vec_remove(&index->entries, idx, NULL) == -1)
        return -1;
      return 0;
    }
  }

  return -1;
}