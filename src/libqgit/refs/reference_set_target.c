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
#include <fileutil.h>
#include <libqgit/error.h>
#include <libqgit/repo/refs.h>
#include <libqgit/repo/repository.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

int qgit_reference_set_target(qgit_reference *ref, const char *target)
{
    assert(ref && target);

    char path[PATH_MAX];
    char content[PATH_MAX];
    char *new_target;

    if (qgit_reference_validate_name(target) < 0)
        return -1;

    if (ref->type != QGIT_REF_SYMBOLIC) {
        qgit_seterror(QGITERR_BADREFTYPE);
        return -1;
    }

    if (snprintf(path, PATH_MAX, "%s/%s", qgit_repository_path(ref->owner),
                 ref->name) >= PATH_MAX) {
        errno = ENAMETOOLONG;
        return -1;
    }

    if (qgit_reference_ensure_parentdir(path) < 0)
        return -1;

    if (snprintf(content, PATH_MAX, "ref: %s\n", target) >= PATH_MAX) {
        errno = ENAMETOOLONG;
        return -1;
    }

    new_target = strdup(target);
    if (!new_target) /* malloc memory ahead to avoid inconsistency between
                        memory and disk */
        return -1;

    if (write_file(path, content, strlen(content)) < 0) {
        free(new_target);
        return -1;
    }

    if (ref->target.symbolic)
        free(ref->target.symbolic);
    ref->target.symbolic = new_target;

    return 0;
}
