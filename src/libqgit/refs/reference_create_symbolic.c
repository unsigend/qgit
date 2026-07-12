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
#include <libqgit/error.h>
#include <libqgit/repo/refs.h>
#include <libqgit/repo/repository.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int qgit_reference_create_symbolic(qgit_reference **out, qgit_repository *repo,
                                   const char *name, const char *target,
                                   int force)
{
    assert(out && repo && name && target);

    char path[PATH_MAX];
    char content[PATH_MAX];
    qgit_reference *ref;

    if (qgit_reference_validate_name(name) < 0 ||
        qgit_reference_validate_name(target) < 0)
        return -1;

    if (snprintf(path, PATH_MAX, "%s/%s", qgit_repository_path(repo), name) >=
        PATH_MAX) {
        errno = ENAMETOOLONG;
        return -1;
    }

    if (file_exists(path) && !force) {
        qgit_seterror(QGITERR_REFEXISTS);
        return -1;
    }

    if (qgit_reference_ensure_parentdir(path) < 0)
        return -1;

    if (snprintf(content, PATH_MAX, "ref: %s\n", target) >= PATH_MAX) {
        errno = ENAMETOOLONG;
        return -1;
    }

    if (write_file(path, content, strlen(content)) < 0)
        return -1;

    ref = calloc(1, sizeof(struct qgit_reference));
    if (!ref) {
        unlink(path);
        return -1;
    }
    ref->type = QGIT_REF_SYMBOLIC;
    ref->owner = repo;
    ref->name = strdup(name);
    if (!ref->name) {
        qgit_reference_free(ref);
        unlink(path);
        return -1;
    }
    ref->target.symbolic = strdup(target);
    if (!ref->target.symbolic) {
        qgit_reference_free(ref);
        unlink(path);
        return -1;
    }
    *out = ref;

    return 0;
}
