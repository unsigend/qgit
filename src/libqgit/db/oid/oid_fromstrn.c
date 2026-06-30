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

#include <string.h>

int qgit_oid_fromstrn(qgit_oid *oid, const char *str, size_t len)
{
    if (len > QGIT_OID_RAWSZ * 2)
        return -1;

    memset(oid->id, 0, QGIT_OID_RAWSZ);
    for (size_t i = 0; i < len; i++) {
        int v = hexval(str[i]);
        if (v == -1)
            return -1;
        if (i % 2 == 0)
            oid->id[i / 2] = (unsigned char)(v << 4);
        else
            oid->id[i / 2] |= (unsigned char)v;
    }
    return 0;
}