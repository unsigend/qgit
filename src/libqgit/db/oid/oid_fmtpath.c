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

#include <libqgit/db/oid.h>
#include <stddef.h>
#include <stdio.h>

int qgit_oid_fmtpath(char *str, const qgit_oid *oid)
{
    char hex[QGIT_OID_HEXSZ];
    if (qgit_oid_fmt(hex, oid) < 0)
        return -1;
    if (snprintf(str, QGIT_OID_HEXSZ + 2, "%c%c/%s", hex[0], hex[1], &hex[2]) <
        QGIT_OID_HEXSZ)
        return -1;
    return 0;
}