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
#include <fs.h>
#include <libqgit/refs.h>
#include <libqgit/repository.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int qgit_reference_set_target(qgit_reference *ref, const char *target)
{
    assert(ref && target);

    if (ref->type != QGIT_REF_SYMBOLIC)
        return -1;

    char path[PATH_MAX];
    void *buf;
    int n = 0;

    if (snprintf(path, PATH_MAX, "%s/%s", qgit_repository_path(ref->owner),
                 ref->name) >= PATH_MAX) {
        errno = ENAMETOOLONG;
        return -1;
    }

    if ((n = snprintf(NULL, 0, "ref: %s\n", target)) < 0)
        return -1;

    buf = malloc(n + 1);
    if (!buf)
        return -1;

    if (snprintf(buf, n + 1, "ref: %s\n", target) < 0) {
        free(buf);
        return -1;
    }

    if (write_file(path, buf, n) == -1) {
        free(buf);
        return -1;
    }

    free(buf);

    if (ref->target.symbolic)
        free(ref->target.symbolic);
    ref->target.symbolic = strdup(target);
    if (!ref->target.symbolic) {
        qgit_reference_free(ref);
        return -1;
    }

    return 0;
}