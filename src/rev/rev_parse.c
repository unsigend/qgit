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

#include <stddef.h>
#include <string.h>

#include "error.h"
#include "rev.h"

/* parse revision format: <base>[^{type}]*/
int rev_parse(const char *name, char *base, enum obj_type *peel,
              enum rev_peel_mode *mode)
{
  if (!name || !base || !peel || !mode)
    return -1;

  size_t len = strlen(name);
  size_t suffixlen;
  const char *end = name + len;
  const char *suffix, *suffix_end;

  suffix = strstr(name, "^{");
  suffix_end = suffix;

  if (!suffix) {
    strcpy(base, name);
    *peel = OBJ_NONE;
    *mode = REV_PEEL_NONE;
    return 0;
  }
  suffix += 2;

  while (suffix_end < end && *suffix_end != '}')
    suffix_end++;

  if (*suffix_end != '}' || suffix_end + 1 != end) {
    setqerrno(QE_BADREV);
    return -1;
  }
  if (suffix == suffix_end) { /* empty suffix */
    memcpy(base, name, len - 3);
    base[len - 3] = '\0';
    *mode = REV_PEEL_DEREF;
    *peel = OBJ_NONE;
    return 0;
  }

  suffixlen = suffix_end - suffix;

  if (strncmp(suffix, "commit", suffixlen) == 0 && suffixlen == 6)
    *peel = OBJ_COMMIT;
  else if (strncmp(suffix, "blob", suffixlen) == 0 && suffixlen == 4)
    *peel = OBJ_BLOB;
  else if (strncmp(suffix, "tree", suffixlen) == 0 && suffixlen == 4)
    *peel = OBJ_TREE;
  else if (strncmp(suffix, "tag", suffixlen) == 0 && suffixlen == 3)
    *peel = OBJ_TAG;
  else {
    setqerrno(QE_BADREV);
    return -1;
  }

  suffix -= 2;
  strncpy(base, name, suffix - name);
  base[suffix - name] = '\0';
  *mode = REV_PEEL_TO;
  return 0;
}