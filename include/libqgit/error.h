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

#define QGITERR_CHECK_ALLOC(p)                                                 \
    if (p == NULL) {                                                           \
        return -1;                                                             \
    }

#define QGITERR_REPO_NOT_FOUND 1 /* repository not found */

BEGIN_DECLS

/* Set the error code */
QGIT_EXTERN(void) qgit_seterrno(int err);

/* Get the error code */
QGIT_EXTERN(int) qgit_geterrno(void);

/* Get the error string */
QGIT_EXTERN(const char *) qgit_error_str(int err);

END_DECLS

#endif