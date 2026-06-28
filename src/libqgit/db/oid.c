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
#include <stdlib.h>
#include <string.h>

static int hexval(unsigned char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    return -1;
}

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

void qgit_oid_fromraw(qgit_oid *oid, const unsigned char *raw)
{
    memcpy(oid->id, raw, QGIT_OID_RAWSZ);
}

int qgit_oid_fmt(char *str, const qgit_oid *oid)
{
    for (size_t i = 0; i < QGIT_OID_RAWSZ; i++)
        if (snprintf(str + i * 2, 3, "%02x", oid->id[i]) < 0)
            return -1;
    str[QGIT_OID_HEXSZ - 1] = '\0';
    return 0;
}

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

char *qgit_oid_strdup(const qgit_oid *oid)
{
    char *str = malloc(QGIT_OID_HEXSZ);
    if (!str)
        return NULL;
    if (qgit_oid_fmt(str, oid) < 0) {
        free(str);
        return NULL;
    }
    return str;
}

void qgit_oid_copy(qgit_oid *dest, const qgit_oid *src)
{
    memcpy(dest->id, src->id, QGIT_OID_RAWSZ);
}

int qgit_oid_cmp(const qgit_oid *a, const qgit_oid *b)
{
    return memcmp(a->id, b->id, QGIT_OID_RAWSZ);
}

int qgit_oid_iszero(const qgit_oid *oid)
{
    qgit_oid zero = {0};
    return memcmp(oid->id, zero.id, QGIT_OID_RAWSZ) == 0;
}