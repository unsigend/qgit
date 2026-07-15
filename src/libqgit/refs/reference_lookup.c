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
#include <fileutils.h>
#include <libqgit/error.h>
#include <libqgit/repo/refs.h>
#include <libqgit/repo/repository.h>
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

    char path[PATH_MAX], target[1024];
    void *buf;
    size_t buflen;
    qgit_reference *reference;

    if (qgit_reference_validate_name(name) < 0)
        return -1;

    if (snprintf(path, PATH_MAX, "%s/%s", qgit_repository_path(repo), name) >=
        PATH_MAX) {
        errno = ENAMETOOLONG;
        return -1;
    }

    if (read_file(path, &buf, &buflen) < 0)
        return -1;

    if (buflen == 0 || buf == NULL) {
        qgit_seterror(QGITERR_BADREFFILE);
        return -1;
    }

    reference = calloc(1, sizeof(qgit_reference));
    if (!reference) {
        free(buf);
        return -1;
    }

    reference->owner = repo;
    reference->name = strdup(name);
    if (!reference->name) {
        qgit_reference_free(reference);
        free(buf);
        return -1;
    }

    if (buflen >= 5 && strncmp(buf, "ref: ", 5) == 0) /* symbolic reference */
    {
        reference->type = QGIT_REF_SYMBOLIC;
        if (sscanf(buf, "ref: %1023s", target) != 1) {
            free(buf);
            qgit_reference_free(reference);
            return -1;
        }
        reference->target.symbolic = strdup(target);
        if (!reference->target.symbolic) {
            free(buf);
            qgit_reference_free(reference);
            return -1;
        }
    } else /* direct reference */
    {
        reference->type = QGIT_REF_DIRECT;
        char *end = buf + buflen;
        while (end > (char *)buf && (*(end - 1) == '\n' || *(end - 1) == '\r'))
            --end;
        *end = '\0';
        if ((size_t)(end - (char *)buf) < QGIT_OID_HEXSZ) {
            free(buf);
            qgit_reference_free(reference);
            qgit_seterror(QGITERR_BADOBJFILE);
            return -1;
        }
        if (qgit_oid_fromstr(&reference->target.oid, buf) < 0) {
            free(buf);
            qgit_reference_free(reference);
            return -1;
        }
    }

    free(buf);
    *out = reference;

    return 0;
}
