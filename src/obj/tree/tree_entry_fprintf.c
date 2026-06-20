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
#include <sys/stat.h>

#include "error.h"
#include "obj/tree.h"

static const char *mode_to_str(mode_t mode)
{
  if (S_ISDIR(mode))
    return "tree";
  else if (S_ISREG(mode) || S_ISLNK(mode))
    return "blob";
  else if (mode == 0160000)
    return "commit";
  else {
    setqerrno(QE_INVALIDOBJ);
    return NULL;
  }
}

int tree_entry_fprintf(struct tree_entry *entry, const char *prefix, FILE *fp)
{
  if (!entry || !fp)
    return -1;

  unsigned char hex[SHA1_HEXLEN];
  const char *modestr = NULL;

  if (!((modestr = mode_to_str(entry->mode))))
    return -1;

  if (sha1_to_hex(entry->sha1, hex) == -1)
    return -1;

  if (fprintf(fp, "%06o %s %s\t%s%s%s\n", entry->mode, modestr, hex,
              prefix ? prefix : "", prefix ? "/" : "", entry->path) < 0)
    return -1;
  return 0;
}