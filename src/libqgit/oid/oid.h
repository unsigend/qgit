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

#ifndef OID_H
#define OID_H

#include <libqgit/common.h>
#include <libqgit/error.h>

QGIT_INLINE(int) hexval(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    return -1;
}

/**
 * Format a single raw byte to a two-character hex string.
 *
 * @param hex The hex string to write to.
 * @param raw The raw byte to format.
 */
QGIT_INLINE(void) qgit_oid_fmt_one(char *hex, unsigned char raw)
{
    static const char hexdigits[] = "0123456789abcdef";
    hex[0] = hexdigits[raw >> 4];
    hex[1] = hexdigits[raw & 0x0f];
}

/**
 * Parse two hex digits into a single raw byte.
 *
 * @param raw  The raw byte to write to.
 * @param high The high nibble hex digit (bits 7-4).
 * @param low  The low nibble hex digit (bits 3-0).
 * @return 0 on success, -1 on error.
 */
QGIT_INLINE(int) qgit_oid_fromstr_one(unsigned char *raw, char high, char low)
{
    int hi = hexval(high);
    int lo = hexval(low);

    if (hi < 0 || lo < 0) {
        qgit_seterror(QGITERR_BADOID);
        return -1;
    }
    *raw = (unsigned char)((hi << 4) | lo);
    return 0;
}

#endif