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

#include <die.h>
#include <errno.h>
#include <feature.h>
#include <libqgit/error.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void die(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "%s: ", PROG_NAME);
    vfprintf(stderr, fmt, args);
    fputc('\n', stderr);
    va_end(args);
    exit(EXIT_FAILURE);
}

void die_errno(void)
{
    fprintf(stderr, "%s: ", PROG_NAME);
    if (qgit_geterrno()) {
        fprintf(stderr, "%s", qgit_error_str(qgit_geterrno()));
    } else if (errno) {
        fprintf(stderr, "%s", strerror(errno));
    } else {
        fprintf(stderr, "unknown error");
    }
    fputc('\n', stderr);
    exit(EXIT_FAILURE);
}