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
#include <libqgit/refs.h>
#include <libqgit/repository.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int qgit_reference_create_oid(qgit_reference **out, qgit_repository *repo,
                              const char *name, const qgit_oid *id, int force)
{
    assert(out && repo && name && id);

    qgit_reference *ref;
    char path[PATH_MAX];
    char hex[QGIT_OID_HEXSZ];

    if (snprintf(path, PATH_MAX, "%s/%s", qgit_repository_path(repo), name) >=
        PATH_MAX) {
        errno = ENAMETOOLONG;
        return -1;
    }

    if (file_exists(path) && !force) {
        qgit_seterrno(QGITERR_REFEXISTS);
        return -1;
    }

    qgit_oid_fmt(hex, id);
    hex[QGIT_OID_HEXSZ - 1] = '\n';
    if (write_file(path, hex, QGIT_OID_HEXSZ) == -1)
        return -1;

    ref = calloc(1, sizeof(qgit_reference));
    if (!ref)
        return -1;
    ref->owner = repo;
    ref->name = strdup(name);
    if (!ref->name) {
        qgit_reference_free(ref);
        return -1;
    }
    ref->type = QGIT_REF_OID;
    qgit_oid_copy(&ref->target.oid, id);

    *out = ref;
    return 0;
}