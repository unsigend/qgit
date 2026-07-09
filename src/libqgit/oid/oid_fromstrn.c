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
    size_t p;

    if (length > QGIT_OID_HEXSZ || !length)
        return -1;

    for (p = 0; p + 1 < length; p += 2) {
        if (qgit_oid_fromstr_one(out->id + p / 2, str[p], str[p + 1]) < 0)
            return -1;
    }

    if (length % 2) {
        int hi = hexval(str[p]);
        if (hi < 0) {
            qgit_seterror(QGITERR_BADOID);
            return -1;
        }
        out->id[p / 2] = (unsigned char)(hi << 4);
        p += 2;
    }

    memset(out->id + p / 2, 0, QGIT_OID_RAWSZ - p / 2);
    return 0;
}
