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
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "obj/sign.h"

char *sign_parse(struct sign *sign, char *buf, char *bufend)
{
  char *cur = buf, *endstr;

  sign->name = cur;
  while (cur < bufend && *cur != '<')
    cur++;
  if (cur >= bufend) {
    setqerrno(QE_BADSIGN);
    return NULL;
  }
  if (cur == buf || *(cur - 1) != ' ') {
    setqerrno(QE_BADSIGN);
    return NULL;
  }
  *(cur - 1) = '\0';

  sign->email = ++cur;
  while (cur < bufend && *cur != '>')
    cur++;
  if (cur >= bufend) {
    setqerrno(QE_BADSIGN);
    return NULL;
  }
  *cur = '\0';
  cur += 2;
  if (cur >= bufend) {
    setqerrno(QE_BADSIGN);
    return NULL;
  }
  errno = 0;
  sign->time = strtoul(cur, &endstr, 10);
  if (errno || endstr == cur || endstr >= bufend || *endstr != ' ') {
    if (!errno)
      setqerrno(QE_BADSIGN);
    return NULL;
  }

  cur = endstr + 1;
  if (cur + SIGN_ZONE_LEN >= bufend) {
    setqerrno(QE_BADSIGN);
    return NULL;
  }
  if (cur[0] != '-' && cur[0] != '+') {
    setqerrno(QE_BADSIGN);
    return NULL;
  }
  memcpy(sign->zone, cur, SIGN_ZONE_LEN);
  sign->zone[SIGN_ZONE_LEN] = '\0';

  while (cur < bufend && *cur != '\n')
    cur++;

  if (cur == bufend) {
    setqerrno(QE_BADSIGN);
    return NULL;
  }

  if (cur < bufend)
    *cur++ = '\0';
  return cur;
}
