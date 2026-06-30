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

#include <stddef.h>

int qgit_oid_fromstr(qgit_oid *oid, const char *str)
{
    for (size_t i = 0; i < QGIT_OID_RAWSZ; i++) {
        int high = hexval(str[i * 2]);
        int low = hexval(str[i * 2 + 1]);
        if (high == -1 || low == -1)
            return -1;
        oid->id[i] = (unsigned char)(high << 4 | low);
    }
    return 0;
}