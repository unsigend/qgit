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
#include <sys/stat.h>

#include "obj/tree.h"

static const char *entry_type_str(mode_t mode)
{
  if (S_ISDIR(mode))
    return "tree";
  /* Keep consistency with git, not supported in qgit */
  else if (mode == 0160000)
    return "commit";
  else if (S_ISREG(mode) || S_ISLNK(mode))
    return "blob";
  else {
    errno = EINVAL;
    return NULL;
  }
}

int tree_entry_fprintf(FILE *stream, struct tree_entry *entry,
                       const char *prefix)
{
  if (!stream || !entry) {
    errno = EINVAL;
    return -1;
  }

  const char *type = entry_type_str(entry->mode);
  if (!type)
    return -1;

  unsigned char hex[SHA1_HEX_LENGTH];
  if (sha1_to_hex(entry->sha1, hex) == -1)
    return -1;

  int ret = 0;
  if (prefix)
    ret = fprintf(stream, "%06o %s %s\t%s/%s\n", entry->mode, type, hex, prefix,
                  entry->path);
  else
    ret = fprintf(stream, "%06o %s %s\t%s\n", entry->mode, type, hex,
                  entry->path);
  return ret >= 0 ? 0 : -1;
}
