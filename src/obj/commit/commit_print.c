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

#include "collection/slist.h"
#include "obj/commit.h"
#include "obj/obj.h"

int commit_fprintf(FILE *stream, struct obj *obj)
{
  if (!stream || !obj) {
    errno = EINVAL;
    return -1;
  }

  unsigned char hex[SHA1_HEX_LENGTH];

  if (sha1_to_hex(obj->commit.tree, hex) == -1)
    return -1;
  fprintf(stream, "tree %s\n", hex);

  if (!slist_empty(&obj->commit.parents)) {
    struct slist_iter iter;
    if (slist_iter_init(&iter, &obj->commit.parents) == -1)
      return -1;
    while (slist_iter_get(&iter)) {
      if (sha1_to_hex(slist_iter_get(&iter), hex) == -1)
        return -1;
      fprintf(stream, "parent %s\n", hex);
      slist_iter_inc(&iter);
    }
  }
  fprintf(stream, "author %s %ld %s\n", obj->commit.author, obj->commit.atime,
          obj->commit.azone);
  fprintf(stream, "committer %s %ld %s\n", obj->commit.committer,
          obj->commit.ctime, obj->commit.czone);
  fputc('\n', stream);
  if (obj->commit.msg)
    fprintf(stream, "%s", obj->commit.msg);
  return 0;
}
