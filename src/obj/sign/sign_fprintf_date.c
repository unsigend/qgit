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

static int date_fmt(time_t time, const char *zone, char *buf, size_t buflen)
{
  if (!zone || !buf || !buflen)
    return -1;

  if (strlen(zone) < 5 || (zone[0] != '-' && zone[0] != '+'))
    return -1;

  struct tm tm;
  int off, sign, oh, om;
  time_t adj;

  sign = (zone[0] == '-') ? -1 : 1;
  oh = (zone[1] - '0') * 10 + (zone[2] - '0');
  om = (zone[3] - '0') * 10 + (zone[4] - '0');
  off = sign * (oh * 60 * 60 + om * 60);

  adj = time + off;
  if (!gmtime_r(&adj, &tm))
    return -1;
  strftime(buf, buflen, "%a %b %e %H:%M:%S %Y", &tm);
  return 0;
}

int sign_fprintf_date(struct sign *sign, FILE *fp)
{
  if (!sign || !fp || !sign->zone)
    return -1;

  char datebuf[64];

  if (date_fmt(sign->time, sign->zone, datebuf, sizeof(datebuf)) == -1)
    return -1;

  if (fprintf(fp, "%s %s", datebuf, sign->zone) < 0)
    return -1;

  return 0;
}