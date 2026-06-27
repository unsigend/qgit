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

#include <sys/stat.h>

#include "obj/tree.h"

int tree_entry_cmp(const void *a, const void *b)
{
  const struct tree_entry *e1 = (const struct tree_entry *)a;
  const struct tree_entry *e2 = (const struct tree_entry *)b;
  const char *p1 = e1->path, *p2 = e2->path;

  while (*p1 && *p2 && *p1 == *p2)
    p1++, p2++;

  char c1 = *p1 ? *p1 : (S_ISDIR(e1->mode) ? '/' : '\0');
  char c2 = *p2 ? *p2 : (S_ISDIR(e2->mode) ? '/' : '\0');
  return (unsigned char)c1 - (unsigned char)c2;
}