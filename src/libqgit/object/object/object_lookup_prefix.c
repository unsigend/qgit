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

#include "object.h"

#include <assert.h>
#include <errno.h>
#include <libqgit/db/odb.h>
#include <libqgit/error.h>
#include <libqgit/object/object.h>
#include <libqgit/repo/repository.h>
#include <limits.h>
#include <stdio.h>

int qgit_object_lookup_prefix(qgit_object **out, qgit_repository *repo,
                              const qgit_oid *id, unsigned int len,
                              qgit_obj_type type)
{
    assert(out && repo && id && len >= QGIT_OID_MINPREFIXLEN);
    *out = NULL;

    char path[PATH_MAX];
    qgit_odb *odb;
    qgit_odb_object *odb_object;
    qgit_object *object;

    if (snprintf(path, PATH_MAX, "%s/objects", qgit_repository_path(repo)) >=
        PATH_MAX) {
        errno = ENAMETOOLONG;
        return -1;
    }

    if (qgit_odb_open(&odb, path) < 0)
        return -1;

    if (qgit_odb_read_prefix(&odb_object, odb, id, len) < 0) {
        qgit_odb_free(odb);
        return -1;
    }

    qgit_odb_free(odb);

    if (type != QGIT_OBJ_ANY && qgit_odb_object_type(odb_object) != type) {
        qgit_odb_object_free(odb_object);
        qgit_seterror(QGITERR_OBJTYPEMISMATCH);
        return -1;
    }

    if (qgit_obj_from_odb_obj(&object, odb_object, repo) < 0) {
        qgit_odb_object_free(odb_object);
        return -1;
    }

    qgit_odb_object_free(odb_object);

    *out = object;

    return 0;
}
