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

#ifndef LIBQGIT_DB_OID_H
#define LIBQGIT_DB_OID_H

#include <libqgit/common.h>
#include <libqgit/types.h>

#define QGIT_OID_RAWSZ 20 /* Size of the raw OID in bytes */
#define QGIT_OID_HEXSZ                                                         \
    (QGIT_OID_RAWSZ * 2 + 1)    /* Size of the hex OID in bytes */
#define QGIT_OID_MINPREFIXLEN 4 /* Minimum prefix length for OID */

struct qgit_oid {
    unsigned char id[QGIT_OID_RAWSZ];
};

BEGIN_DECLS

/* Parse a hex formatted object id into a qgit_oid */
QGIT_EXTERN(int) qgit_oid_fromstr(qgit_oid *oid, const char *str);

/* Parse a raw object id into a qgit_oid */
QGIT_EXTERN(void) qgit_oid_fromraw(qgit_oid *oid, const unsigned char *raw);

/* Format a qgit_oid into a hex formatted string */
QGIT_EXTERN(int) qgit_oid_fmt(char *str, const qgit_oid *oid);

/* Format a qgit_oid into a path formatted string with "xx/..." format. */
QGIT_EXTERN(int) qgit_oid_fmtpath(char *str, const qgit_oid *oid);

/* Duplicate a qgit_oid into a heap allocated hex string. */
QGIT_EXTERN(char *) qgit_oid_strdup(const qgit_oid *oid);

/* Copy a qgit_oid to another qgit_oid. */
QGIT_EXTERN(void) qgit_oid_copy(qgit_oid *dest, const qgit_oid *src);

/* Compare two qgit_oids. */
QGIT_EXTERN(int) qgit_oid_cmp(const qgit_oid *a, const qgit_oid *b);

/* Check if a qgit_oid is zero. Return 1 if zero, 0 otherwise. */
QGIT_EXTERN(int) qgit_oid_iszero(const qgit_oid *oid);

END_DECLS

#endif