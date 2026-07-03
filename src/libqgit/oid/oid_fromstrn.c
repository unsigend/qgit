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
#include <string.h>

int qgit_oid_fromstrn(qgit_oid *out, const char *str, size_t length)
{
    if (length > QGIT_OID_HEXSZ || !length)
        return -1;

    memset(out->id, 0, QGIT_OID_RAWSZ);

    for (size_t i = 0; i < length / 2; i++) /* parse N - 1 if N is odd */
    {
        if (qgit_oid_fromstr_one(out->id + i, str[i * 2], str[i * 2 + 1]))
            return -1;
    }
    return 0;
}
