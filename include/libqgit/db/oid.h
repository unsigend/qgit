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
#include <stddef.h>

#define QGIT_OID_RAWSZ 20                       /* Raw binary oid length */
#define QGIT_OID_HEXSZ (QGIT_OID_RAWSZ * 2 + 1) /* Hex formatted oid length */
#define QGIT_OID_MINPREFIXLEN 4 /* Minimum length of an oid prefix */

/* Unique identity of any object (commit, tree, blob, tag). */
struct qgit_oid {
    unsigned char id[QGIT_OID_RAWSZ]; /* Raw binary oid */
};

BEGIN_DECLS

/**
 * Parse a hex formatted object id into a qgit_oid.
 *
 * @param oid oid structure the result is written into
 * @param str input hex string, must be pointing at the start of
 *		the hex sequence and have at least the number of bytes
 *		needed for an oid encoded in hex (40 bytes)
 * @return 0 on success, -1 on error and set errno
 */
QGIT_EXTERN(int) qgit_oid_fromstr(qgit_oid *oid, const char *str);

/**
 * Parse N characters of a hex formatted object id into a qgit_oid.
 *
 * If N is odd, N-1 characters will be parsed instead.
 * The remaining space in the qgit_oid will be set to zero.
 * Used to build a prefix OID for short SHA lookup.
 *
 * @param oid oid structure the result is written into
 * @param str input hex string of at least size `len`
 * @param len length of the input string
 * @return 0 on success, -1 on error and set errno
 */
QGIT_EXTERN(int) qgit_oid_fromstrn(qgit_oid *oid, const char *str, size_t len);

/**
 * Copy an already raw oid into a qgit_oid structure.
 *
 * @param oid oid structure the result is written into
 * @param raw the raw input bytes to be copied
 */
QGIT_EXTERN(void) qgit_oid_fromraw(qgit_oid *oid, const unsigned char *raw);

/**
 * Format a qgit_oid into a hex string.
 *
 * @param str output hex string, must be pointing at the start of
 *		the hex sequence and have at least the number of bytes
 *		needed for an oid encoded in hex (41 bytes). Only the
 *		oid digits are written, a '\\0' terminator must be added
 *		by the caller if it is required
 * @param oid oid structure to format
 * @return 0 on success, -1 on error and set errno
 */
QGIT_EXTERN(int) qgit_oid_fmt(char *str, const qgit_oid *oid);

/**
 * Format a qgit_oid into a loose object path string.
 *
 * The resulting string is "aa/...", where "aa" is the first two
 * hex digits of the oid and "..." is the remaining 38 digits.
 *
 * @param str output hex string, must be pointing at the start of
 *		the hex sequence and have at least the number of bytes
 *		needed for an oid encoded in hex (42 bytes). Only the
 *		oid digits are written, a '\\0' terminator must be added
 *		by the caller if it is required
 * @param oid oid structure to format
 * @return 0 on success, -1 on error and set errno
 */
QGIT_EXTERN(int) qgit_oid_fmtpath(char *str, const qgit_oid *oid);

/**
 * Format a qgit_oid into a newly allocated c-string.
 *
 * @param oid the oid structure to format
 * @return the c-string on success, NULL on error and set errno. Caller must
 *			deallocate the string with free()
 */
QGIT_EXTERN(char *) qgit_oid_strdup(const qgit_oid *oid);

/**
 * Copy an oid from one structure to another.
 *
 * @param dest oid structure the result is written into
 * @param src oid structure to copy from
 */
QGIT_EXTERN(void) qgit_oid_copy(qgit_oid *dest, const qgit_oid *src);

/**
 * Compare two oid structures.
 *
 * @param a first oid structure
 * @param b second oid structure
 * @return <0, 0, >0 if a < b, a == b, a > b
 */
QGIT_EXTERN(int) qgit_oid_cmp(const qgit_oid *a, const qgit_oid *b);

/**
 * Compare the first `len` hexadecimal characters (packets of 4 bits)
 * of two oid structures.
 *
 * @param a first oid structure
 * @param b second oid structure
 * @param len the number of hex chars to compare
 * @return 0 in case of a match
 */
QGIT_EXTERN(int)
qgit_oid_ncmp(const qgit_oid *a, const qgit_oid *b, size_t len);

/**
 * Check if an oid is all zeros.
 *
 * @param oid oid structure to test
 * @return 1 if the oid is zero, 0 otherwise
 */
QGIT_EXTERN(int) qgit_oid_iszero(const qgit_oid *oid);

END_DECLS

#endif
