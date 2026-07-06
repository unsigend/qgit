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

#ifndef ERROR_H
#define ERROR_H

#include <libqgit/common.h>

#define QGITERR_BADOID 1          /* Bad OID */
#define QGITERR_REPONOTFOUND 2    /* Repository not found */
#define QGITERR_INVKEY 3          /* Invalid key */
#define QGITERR_BADOBJFILE 4      /* Bad object file */
#define QGITERR_BADOBJTYPE 5      /* Bad object type */
#define QGITERR_OBJNOTFOUND 6     /* Object not found */
#define QGITERR_AMBIGUOUS 7       /* Ambiguous object */
#define QGITERR_OBJTYPEMISMATCH 8 /* Object type mismatch */

/**
 * Set the error code.
 *
 * @param err The error code to set.
 */
QGIT_EXTERN(void) qgit_seterror(int err);

/**
 * Get the error code.
 *
 * @return The error code.
 */
QGIT_EXTERN(int) qgit_error(void);

/**
 * Clear the error code.
 */
QGIT_EXTERN(void) qgit_clear_error(void);

/**
 * Get the error message.
 *
 * @return The error message.
 */
QGIT_EXTERN(const char *) qgit_strerror(int err);

#endif