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

#ifndef ERROR_H
#define ERROR_H

#include <errno.h>

#define QE_NOTINREPO 1
#define QE_BADOBJFILE 2
#define QE_INTERNAL 3
#define QE_AMBIGUOUS 4

/* wrapper for get_qerror(), follow ANSI/ISO C errno design pattern.*/
extern int *qerrno_location(void);
#define qerrno (*qerrno_location())
#define setqerrno(code)                                                        \
  errno = 0;                                                                   \
  qerrno = code;

extern const char *qerror_str(int error);

#endif