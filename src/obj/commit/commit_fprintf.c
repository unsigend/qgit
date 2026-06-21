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

#include "collection/slist.h"
#include "obj/commit.h"
#include "obj/object.h"
#include "sha1.h"

int commit_fprintf(struct obj *obj, FILE *fp)
{
  if (!obj || !fp || obj->type != OBJ_COMMIT)
    return -1;

  unsigned char hex[SHA1_HEXLEN];
  struct slist_iter iter;
  struct sign *authorsign = &obj->commit.author;
  struct sign *comsign = &obj->commit.committer;

  if (sha1_to_hex(obj->commit.tree, hex) == -1)
    return -1;

  if (fprintf(fp, "tree %s\n", hex) < 0)
    return -1;

  if (slist_iter_init(&iter, &obj->commit.parents) == -1)
    return -1;

  while (slist_iter_get(&iter)) {
    if (sha1_to_hex((unsigned char *)slist_iter_get(&iter), hex) == -1)
      return -1;
    if (fprintf(fp, "parent %s\n", hex) < 0)
      return -1;
    slist_iter_inc(&iter);
  }

  if (authorsign->name && authorsign->email && authorsign->zone) {
    if (fprintf(fp, "author ") < 0 || sign_fprintf_name(authorsign, fp) == -1 ||
        fprintf(fp, " %ld %s\n", authorsign->time, authorsign->zone) < 0)
      return -1;
  }
  if (comsign->name && comsign->email && comsign->zone) {
    if (fprintf(fp, "committer ") < 0 || sign_fprintf_name(comsign, fp) == -1 ||
        fprintf(fp, " %ld %s\n", comsign->time, comsign->zone) < 0)
      return -1;
  }
  if (fputc('\n', fp) < 0)
    return -1;
  if (obj->commit.msg) {
    if (fprintf(fp, "%s", obj->commit.msg) < 0)
      return -1;
  }

  return 0;
}