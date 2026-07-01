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
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int qgit_reference_lookup(qgit_reference **out, qgit_repository *repo,
                          const char *name)
{
    assert(out && repo && name);

    *out = NULL;

    qgit_reference *ref = NULL;
    char path[PATH_MAX];
    void *buf;
    size_t len;

    if (snprintf(path, PATH_MAX, "%s/%s", qgit_repository_path(repo), name) >=
        PATH_MAX) {
        errno = ENAMETOOLONG;
        return -1;
    }

    ref = calloc(1, sizeof(qgit_reference));
    if (!ref)
        return -1;
    ref->owner = repo;
    ref->name = strdup(name);
    if (!ref->name) {
        qgit_reference_free(ref);
        return -1;
    }

    if (read_file(path, &buf, &len) == -1) {
        qgit_reference_free(ref);
        return -1;
    }

    if (len == 0) {
        qgit_reference_free(ref);
        qgit_seterrno(QGITERR_BADREF);
        return -1;
    }

    if (strncmp(buf, "ref: ", 5) == 0) /* symbolic reference */
    {
        if (sscanf(buf, "ref: %s", path) != 1) {
            qgit_reference_free(ref);
            free(buf);
            return -1;
        }
        free(buf);
        ref->type = QGIT_REF_SYMBOLIC;
        ref->target.symbolic = strdup(path);
        if (!ref->target.symbolic) {
            qgit_reference_free(ref);
            return -1;
        }
    } else /* direct reference */
    {
        qgit_oid_fromstr(&ref->target.oid, (char *)buf);
        ref->type = QGIT_REF_OID;
        free(buf);
    }

    *out = ref;
    return 0;
}