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
#include <time.h>

#include "obj/sign.h"

int sign_date_fmt(const struct sign *sign, char *buf, size_t buflen)
{
  if (!sign || !buf || !buflen)
    return -1;

  struct tm tm;
  int off, s, oh, om;
  time_t adj;

  s = (sign->zone[0] == '-') ? -1 : 1;
  oh = (sign->zone[1] - '0') * 10 + (sign->zone[2] - '0');
  om = (sign->zone[3] - '0') * 10 + (sign->zone[4] - '0');
  off = s * (oh * 60 * 60 + om * 60);

  adj = sign->time + off;
  if (!gmtime_r(&adj, &tm))
    return -1;
  if (!strftime(buf, buflen, "%a %b %e %H:%M:%S %Y", &tm))
    return -1;
  return 0;
}
