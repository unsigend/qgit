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

#include <string.h>
#include <time.h>

#include "obj/sign.h"

int sign_init_now(struct sign *sign, const char *name, const char *email)
{
  if (!sign || !name || !email)
    return -1;

  time_t now = time(NULL);
  struct tm tm;

  if (!localtime_r(&now, &tm))
    return -1;
  if (!strftime(sign->zone, sizeof(sign->zone), "%z", &tm))
    return -1;
  if (strlen(sign->zone) != SIGN_ZONE_LEN ||
      (sign->zone[0] != '-' && sign->zone[0] != '+'))
    return -1;

  sign->name = name;
  sign->email = email;
  sign->time = now;
  return 0;
}