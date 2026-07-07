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
#include <stddef.h>

static int qgit_error_code = 0; /* error code */

static const char *qgit_error_messages[] = {
    [QGITERR_BADOID] = "bad OID",
    [QGITERR_REPONOTFOUND] = "not inside a qgit repository",
    [QGITERR_INVKEY] = "invalid key format",
    [QGITERR_BADOBJFILE] = "bad object file",
    [QGITERR_BADOBJTYPE] = "bad object type",
    [QGITERR_OBJNOTFOUND] = "object not found",
    [QGITERR_AMBIGUOUS] = "ambiguous object",
    [QGITERR_OBJTYPEMISMATCH] = "object type mismatch",
    [QGITERR_BADREFNAME] = "bad reference name",
    [QGITERR_BADREFFILE] = "bad reference file",
    [QGITERR_BADREFTYPE] = "bad reference type",
    [QGITERR_REFEXISTS] = "reference already exists",
    [QGITERR_BRANCHEXISTS] = "branch already exists",
    [QGITERR_BADSIGNATURE] = "bad signature",
    [QGITERR_BADCOMMITFILE] = "bad commit file",
    [QGITERR_BADTREEFILE] = "bad tree file",
    [QGITERR_NOSUBTREE] = "subtree not found",
};

void qgit_seterror(int err) { qgit_error_code = err; }
int qgit_error(void) { return qgit_error_code; }
void qgit_clear_error(void) { qgit_error_code = 0; }

const char *qgit_strerror(int err)
{
    if (err < 0 || (size_t)err >= sizeof(qgit_error_messages) /
                                      sizeof(qgit_error_messages[0]))
        return "Unknown error";
    return qgit_error_messages[err];
}