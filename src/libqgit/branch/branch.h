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

#ifndef BRANCH_H
#define BRANCH_H

#include <libqgit/error.h>
#include <string.h>

/**
 * Validate a branch name.
 *
 * @param name The branch name to validate.
 * @return 0 if the name is valid, -1 if the name is invalid.
 */
QGIT_INLINE(int) qgit_branch_validate_name(const char *name)
{
    if (!name[0] || strstr(name, "..") || name[0] == '/') {
        qgit_seterror(QGITERR_BADREFNAME);
        return -1;
    }
    return 0;
}

#endif