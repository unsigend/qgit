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

#ifndef REFERENCE_H
#define REFERENCE_H

#include <errno.h>
#include <fileutils.h>
#include <libqgit/error.h>
#include <libqgit/oid.h>
#include <libqgit/types.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

struct qgit_reference {
    qgit_ref_type type;
    qgit_repository *owner;
    char *name; /* full reference name */

    union {
        qgit_oid oid;   /* QGIT_REF_OID */
        char *symbolic; /* QGIT_REF_SYMBOLIC */
    } target;
};

/**
 * Validate a reference name.
 *
 * @param name The reference name to validate.
 * @return 0 if the name is valid, -1 if the name is invalid.
 */
QGIT_INLINE(int) qgit_reference_validate_name(const char *name)
{
    if ((strcmp(name, "HEAD") != 0 && strncmp(name, "refs/", 5) != 0) ||
        strstr(name, "..")) {
        qgit_seterror(QGITERR_BADREFNAME);
        return -1;
    }
    return 0;
}

/**
 * Ensure the parent directory of a reference path exists. Create it if it does
 * not exist.
 *
 * @param path The reference path to ensure the parent directory exists.
 * @return 0 if successful, -1 if failed.
 */
QGIT_INLINE(int)
qgit_reference_ensure_parentdir(const char *path)
{
    char parent[PATH_MAX];
    char *slash;

    if (snprintf(parent, PATH_MAX, "%s", path) >= PATH_MAX) {
        errno = ENAMETOOLONG;
        return -1;
    }

    slash = strrchr(parent, '/');
    if (!slash) {
        errno = EINVAL;
        return -1;
    }
    if (slash == parent)
        parent[1] = '\0';
    else
        *slash = '\0';
    if (mkdirp(parent, QGIT_DIRMODE) < 0)
        return -1;
    return 0;
}

#endif