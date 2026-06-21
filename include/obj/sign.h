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

#ifndef OBJ_SIGN_H
#define OBJ_SIGN_H

#include <stdio.h>
#include <time.h>

struct sign {
  const char *name;
  const char *email;
  time_t time;
  const char *zone;
};

/* parse signature inplace from buffer: <name> <email> <timestamp> <timezone>,
   return next pointer */
extern char *sign_parse(struct sign *sign, char *buf, char *bufend);

extern int sign_fprintf_date(struct sign *sign, FILE *fp);
extern int sign_fprintf_name(struct sign *sign, FILE *fp);

#endif