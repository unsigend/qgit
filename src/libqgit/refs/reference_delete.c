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

#include "reference.h"

#include <assert.h>
#include <errno.h>
#include <libqgit/repo/refs.h>
#include <libqgit/repo/repository.h>
#include <limits.h>
#include <stdio.h>
#include <unistd.h>

int qgit_reference_delete(qgit_reference *ref)
{
    assert(ref);

    char path[PATH_MAX];

    if (snprintf(path, PATH_MAX, "%s/%s", qgit_repository_path(ref->owner),
                 ref->name) >= PATH_MAX) {
        errno = ENAMETOOLONG;
        return -1;
    }

    if (unlink(path) < 0)
        return -1;

    qgit_reference_free(ref);

    return 0;
}
