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

#ifndef LIBQGIT_OID_H
#define LIBQGIT_OID_H

#include <libqgit/common.h>
#include <libqgit/types.h>
#include <stddef.h>

QGIT_BEGIN_DECLS

#define QGIT_OID_RAWSZ 20 /* size in bytes of a raw binary oid */
#define QGIT_OID_HEXSZ                                                         \
    (QGIT_OID_RAWSZ * 2)        /* size in bytes of a hex formatted oid */
#define QGIT_OID_MINPREFIXLEN 4 /* minimum hex chars for a valid oid prefix */

/* Unique identity of any object (commit, tree, blob, tag). */
struct qgit_oid {
    unsigned char id[QGIT_OID_RAWSZ]; /* raw binary SHA-1 digest */
};

/**
 * Parse a full 40-character hex string into a qgit_oid.
 *
 * @param out oid structure the result is written into, must not be NULL
 * @param str input hex string, must point at the start of the hex sequence
 *            and contain at least QGIT_OID_HEXSZ characters
 * @return 0 on success, -1 if str is not a valid hex oid
 */
QGIT_EXTERN(int) qgit_oid_fromstr(qgit_oid *out, const char *str);

/**
 * Parse the first N hex characters of a string into a qgit_oid.
 *
 * If N is odd, N-1 characters are parsed instead. The remaining bytes
 * in the oid are zeroed.
 *
 * @param out    oid structure the result is written into, must not be NULL
 * @param str    input hex string of at least length characters
 * @param length number of hex characters to parse, must be <= QGIT_OID_HEXSZ
 * @return 0 on success, -1 on error
 */
QGIT_EXTERN(int)
qgit_oid_fromstrn(qgit_oid *out, const char *str, size_t length);

/**
 * Copy raw bytes into a qgit_oid.
 *
 * @param out oid structure the result is written into, must not be NULL
 * @param raw pointer to at least QGIT_OID_RAWSZ bytes to copy
 */
QGIT_EXTERN(void) qgit_oid_fromraw(qgit_oid *out, const unsigned char *raw);

/**
 * Format a qgit_oid as a hex string without a null terminator.
 *
 * The caller must supply a buffer of at least QGIT_OID_HEXSZ bytes and
 * append the null terminator if required.
 *
 * @param str output buffer of at least QGIT_OID_HEXSZ bytes, must not be NULL
 * @param oid oid to format, must not be NULL
 */
QGIT_EXTERN(void) qgit_oid_fmt(char *str, const qgit_oid *oid);

/**
 * Format a qgit_oid as a loose-object path of the form "aa/bbbb...".
 *
 * The first two hex digits become the directory component followed by
 * a slash, and the remaining 38 digits form the file name component.
 * No null terminator is written, the caller must supply a buffer of at
 * least QGIT_OID_HEXSZ+1 bytes and append it if needed.
 *
 * @param str output buffer of at least QGIT_OID_HEXSZ+1 bytes, must not be NULL
 * @param oid oid to format, must not be NULL
 */
QGIT_EXTERN(void) qgit_oid_pathfmt(char *str, const qgit_oid *oid);

/**
 * Format a qgit_oid into a null-terminated hex string written to a
 * caller-supplied buffer.
 *
 * If n <= QGIT_OID_HEXSZ the string is truncated to n-1 characters. On
 * invalid input (out == NULL, n == 0, oid == NULL) a pointer to an empty
 * string is returned so the result can always be printed safely.
 *
 * @param out output buffer, must not be NULL
 * @param n   size of out in bytes including the null terminator
 * @param oid oid to format, must not be NULL
 * @return out, or a pointer to an empty string on invalid input
 */
QGIT_EXTERN(char *) qgit_oid_tostr(char *out, size_t n, const qgit_oid *oid);

/**
 * Copy one qgit_oid into another.
 *
 * @param out destination oid, must not be NULL
 * @param src source oid, must not be NULL
 */
QGIT_EXTERN(void) qgit_oid_cpy(qgit_oid *out, const qgit_oid *src);

/**
 * Compare two qgit_oid values lexicographically.
 *
 * @param a first oid, must not be NULL
 * @param b second oid, must not be NULL
 * @return <0 if a < b, 0 if a == b, >0 if a > b
 */
QGIT_EXTERN(int) qgit_oid_cmp(const qgit_oid *a, const qgit_oid *b);

/**
 * Compare the first len hex characters of two qgit_oid values.
 *
 * Used for abbreviated OID prefix matching.
 *
 * @param a   first oid, must not be NULL
 * @param b   second oid, must not be NULL
 * @param len number of hex characters to compare, clamped to QGIT_OID_HEXSZ
 * @return 0 if the first len hex characters match, non-zero otherwise
 */
QGIT_EXTERN(int)
qgit_oid_ncmp(const qgit_oid *a, const qgit_oid *b, unsigned int len);

/**
 * Check whether a qgit_oid equals a hex-formatted string.
 *
 * @param a   oid to compare, must not be NULL
 * @param str null-terminated hex string of a full object id
 * @return 0 if equal, -1 if str is not a valid hex oid, 1 if not equal
 */
QGIT_EXTERN(int) qgit_oid_streq(const qgit_oid *a, const char *str);

/**
 * Test whether a qgit_oid is the all-zeros null oid.
 *
 * @param a oid to test, must not be NULL
 * @return 1 if all bytes are zero, 0 otherwise
 */
QGIT_EXTERN(int) qgit_oid_iszero(const qgit_oid *a);

QGIT_END_DECLS

#endif
