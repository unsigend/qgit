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
#include "ref.h"
#include "repo.h"

struct obj *obj_find(struct repo *repo, const char *name)
{
  if (!repo || !name)
    return NULL;

  unsigned char sha1[SHA1_DIGLEN];
  if (ref_resolve(repo, name, sha1) == -1)
    return NULL;

  return obj_open(repo, sha1);
}