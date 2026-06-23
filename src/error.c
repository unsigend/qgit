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

#include "error.h"

static int __qerrno = 0;

int *qerrno_location(void) { return &__qerrno; }

static const char *errstr[] = {
    [QE_NOTINREPO] = "not inside a qgit repository",
    [QE_BADOBJFILE] = "bad object file",
    [QE_INTERNAL] = "internal error",
    [QE_AMBIGUOUS] = "ambiguous argument",
    [QE_EXISTSTAG] = "tag already exists",
    [QE_BADSIGN] = "bad signature field",
    [QE_IDENTITY] = "identity is required",
    [QE_PEEL] = "could not peel object",
    [QE_BADREV] = "bad revision",
};

const char *qerror_str(int error)
{
  if (error < 1 || error >= (int)(sizeof(errstr) / sizeof(errstr[0])))
    return "unknown error";

  return errstr[error];
}
