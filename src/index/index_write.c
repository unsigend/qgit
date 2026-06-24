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
#include <string.h>

#include "index.h"

static int entry_cmp(const void *a, const void *b)
{
  const struct index_entry *e1 = (const struct index_entry *)a;
  const struct index_entry *e2 = (const struct index_entry *)b;
  int r = strcmp(e1->path, e2->path);
  if (r)
    return r;
  return (int)e1->stage - (int)e2->stage;
}

int index_write(struct index *index)
{
  if (!index)
    return -1;

  vec_sort(&index->entries, entry_cmp);

  /* TODO: the actual fmt and write logic */
  return 0;
}