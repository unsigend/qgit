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

#ifndef LIBQGIT_ERROR_H
#define LIBQGIT_ERROR_H

#include <libqgit/common.h>

#define QGITERR_REPO_NOT_FOUND 1    /* Repository not found */
#define QGITERR_BADOBJFILE 2        /* Bad object file */
#define QGITERR_INVALIDOBJTYPE 3    /* Invalid object type */
#define QGITERR_OBJ_NOT_FOUND 4     /* Object not found */
#define QGITERR_OBJ_TYPE_MISMATCH 5 /* Object type mismatch */
#define QGITERR_AMBIGUOUS 6         /* Ambiguous object */
#define QGITERR_INVKEY 7            /* Invalid key */
#define QGITERR_BADREF 8            /* Bad reference */
#define QGITERR_REFEXISTS 9         /* Reference already exists */
#define QGITERR_REF_NOT_FOUND 10    /* Reference not found */

BEGIN_DECLS

/**
 * Clear the current thread error code set by qgit_seterrno().
 */
QGIT_EXTERN(void) qgit_clearerrno(void);

/**
 * Set the current thread error code.
 *
 * Functions that return -1 call this to record the failure reason.
 * Use qgit_geterrno() to retrieve the code and qgit_error_str() for
 * a human readable message.
 *
 * @param err error code to set
 */
QGIT_EXTERN(void) qgit_seterrno(int err);

/**
 * Get the current thread error code.
 *
 * @return the current error code, or 0 if no error has occurred
 */
QGIT_EXTERN(int) qgit_geterrno(void);

/**
 * Get the human readable string for an error code.
 *
 * @param err error code to look up
 * @return the error string
 */
QGIT_EXTERN(const char *) qgit_error_str(int err);

END_DECLS

#endif
