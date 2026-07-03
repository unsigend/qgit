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

#include "oid.h"

#include <libqgit/oid.h>
#include <stddef.h>

char *qgit_oid_tostr(char *out, size_t n, const qgit_oid *oid)
{
    static const char hexdigits[] = "0123456789abcdef";
    size_t hexlen;
    size_t i;

    if (!out || !n || !oid)
        return "";

    hexlen = n - 1;
    if (hexlen > QGIT_OID_HEXSZ)
        hexlen = QGIT_OID_HEXSZ;

    for (i = 0; i < hexlen / 2; i++)
        qgit_oid_fmt_one(out + i * 2, oid->id[i]);
    if (hexlen % 2)
        out[hexlen - 1] = hexdigits[oid->id[hexlen / 2] >> 4];
    out[hexlen] = '\0';
    return out;
}
