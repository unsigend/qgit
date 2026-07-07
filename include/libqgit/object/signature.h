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

#ifndef LIBQGIT_OBJECT_SIGNATURE_H
#define LIBQGIT_OBJECT_SIGNATURE_H

#include <libqgit/common.h>
#include <libqgit/types.h>
#include <time.h>

QGIT_BEGIN_DECLS

/* UTC timestamp paired with a timezone offset. */
struct qgit_time {
    time_t time; /* seconds since the Unix epoch */
    int offset;  /* timezone offset in minutes east of UTC */
};

/* Author or committer identity attached to a commit or tag. */
struct qgit_signature {
    char *name;     /* full name */
    char *email;    /* email address */
    qgit_time when; /* time */
};

/**
 * Allocate a new signature with an explicit timestamp.
 *
 * Copies of name and email are stored internally. The caller must free
 * the signature with qgit_signature_free when it is no longer needed.
 *
 * @param out    output pointer to receive the new signature, must not be NULL
 * @param name   full name of the person, must not be NULL
 * @param email  email address of the person, must not be NULL
 * @param time   seconds since the Unix epoch
 * @param offset timezone offset in minutes east of UTC
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int)
qgit_signature_new(qgit_signature **out, const char *name, const char *email,
                   time_t time, int offset);

/**
 * Allocate a new signature using the current wall-clock time.
 *
 * The timezone offset is derived from the local system timezone.
 * Copies of name and email are stored internally. The caller must free
 * the signature with qgit_signature_free when it is no longer needed.
 *
 * @param out   output pointer to receive the new signature, must not be NULL
 * @param name  full name of the person, must not be NULL
 * @param email email address of the person, must not be NULL
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int)
qgit_signature_now(qgit_signature **out, const char *name, const char *email);

/**
 * Duplicate an existing signature.
 *
 * All internal strings are deep-copied. The caller must free the returned
 * signature with qgit_signature_free.
 *
 * @param sig source signature to copy, must not be NULL
 * @return a new signature, or NULL on allocation failure
 */
QGIT_EXTERN(qgit_signature *) qgit_signature_dup(const qgit_signature *sig);

/**
 * Free a signature and all internally allocated strings.
 *
 * @param sig signature to free, no-op if NULL
 */
QGIT_EXTERN(void) qgit_signature_free(qgit_signature *sig);

/**
 * Parse a signature from a string. Modify in place.
 *
 * @param out output pointer to receive the new signature, must not be NULL
 * @param start start of the string to parse
 * @param end end of the string to parse
 * @return pointer to the next character after the signature, or NULL on error
 */
QGIT_EXTERN(char *)
qgit_signature_parse(qgit_signature **out, char *start, char *end);

QGIT_END_DECLS

#endif
