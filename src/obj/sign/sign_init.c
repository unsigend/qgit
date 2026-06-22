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

#include "obj/sign.h"

int sign_init(struct sign *sign, const char *name, const char *email,
              time_t time, const char *zone)
{
  if (!sign || !name || !email || !zone)
    return -1;

  if (strlen(zone) != SIGN_ZONE_LEN || (zone[0] != '-' && zone[0] != '+'))
    return -1;

  sign->name = name;
  sign->email = email;
  sign->time = time;
  memcpy(sign->zone, zone, SIGN_ZONE_LEN);
  sign->zone[SIGN_ZONE_LEN] = '\0';
  return 0;
}