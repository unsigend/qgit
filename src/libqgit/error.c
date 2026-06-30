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

#include <libqgit/error.h>

static int qgit_errno = 0;

static const char *errstr[] = {
    [QGITERR_REPO_NOT_FOUND] = "not inside a qgit repository",
    [QGITERR_BADOBJFILE] = "bad object file",
    [QGITERR_INVALIDOBJTYPE] = "invalid object type",
    [QGITERR_OBJ_NOT_FOUND] = "object not found",
    [QGITERR_OBJ_TYPE_MISMATCH] = "object type mismatch",
    [QGITERR_AMBIGUOUS] = "ambiguous object",
};

void qgit_clearerrno(void) { qgit_errno = 0; }
void qgit_seterrno(int err) { qgit_errno = err; }

int qgit_geterrno(void) { return qgit_errno; }

const char *qgit_error_str(int err)
{
    if (err < 1 || err >= (int)(sizeof(errstr) / sizeof(errstr[0])))
        return "internal error";

    return errstr[err];
}