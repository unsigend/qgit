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

#include <assert.h>
#include <errno.h>
#include <libqgit/error.h>
#include <libqgit/object/object.h>
#include <libqgit/object/tag.h>
#include <libqgit/oid.h>
#include <libqgit/repo/branch.h>
#include <libqgit/repo/refs.h>
#include <libqgit/repo/repository.h>
#include <libqgit/revparse.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum revision_type {
    REV_NONE,
    REV_PEEL,
    REV_RESOLVE,
};

static int revparse(const char *spec, enum revision_type *type,
                    qgit_obj_type *obj_type, char **base)
{
    assert(spec && type && obj_type && base);

    const char *suffix = strstr(spec, "^{");
    const char *p;

    if (!suffix) {
        *type = REV_NONE;
        *base = strdup(spec);
        if (!*base)
            return -1;
        return 0;
    }

    p = suffix + 2;
    const char *end = strchr(p, '}');

    if (!end || *(end + 1) != '\0') {
        qgit_seterror(QGITERR_BADREV);
        return -1;
    }

    if (p == end)
        *type = REV_RESOLVE;
    else {
        *type = REV_PEEL;
        if ((size_t)(end - p) == 6 && memcmp(p, "commit", 6) == 0)
            *obj_type = QGIT_OBJ_COMMIT;
        else if ((size_t)(end - p) == 4 && memcmp(p, "tree", 4) == 0)
            *obj_type = QGIT_OBJ_TREE;
        else if ((size_t)(end - p) == 3 && memcmp(p, "tag", 3) == 0)
            *obj_type = QGIT_OBJ_TAG;
        else if ((size_t)(end - p) == 4 && memcmp(p, "blob", 4) == 0)
            *obj_type = QGIT_OBJ_BLOB;
        else {
            qgit_seterror(QGITERR_BADREV);
            return -1;
        }
    }

    *base = strndup(spec, suffix - spec);
    if (!*base)
        return -1;
    return 0;
}

static int resolve_oid(qgit_oid *out, qgit_repository *repo, const char *base)
{
    assert(out && base);

    if (strlen(base) == QGIT_OID_HEXSZ) /* full 40-character hex SHA-1 */
    {
        if (qgit_oid_fromstr(out, base) < 0)
            return -1;
        return 0;
    }

    if (strcmp(base, "HEAD") == 0 ||
        strncmp(base, "refs/", 5) == 0) /* HEAD or full reference path */
    {
        if (qgit_reference_name_to_oid(out, repo, base) < 0)
            return -1;
        return 0;
    }

    qgit_reference *branch = NULL;
    qgit_reference *tag = NULL;
    qgit_oid branch_oid, tag_oid;
    char tagname[PATH_MAX];
    int found = 0;

    if (qgit_branch_lookup(&branch, repo, base, QGIT_BRANCH_LOCAL) ==
        0) /* local branch */
    {
        if (qgit_reference_name_to_oid(&branch_oid, repo,
                                       qgit_reference_name(branch)) < 0) {
            qgit_reference_free(branch);
            return -1;
        }
        qgit_oid_cpy(out, &branch_oid);
        found = 1;
    }

    if (snprintf(tagname, PATH_MAX, "refs/tags/%s", base) >= PATH_MAX) {
        qgit_reference_free(branch);
        errno = ENAMETOOLONG;
        return -1;
    }

    if (qgit_reference_lookup(&tag, repo, tagname) == 0) /* tag */
    {
        if (qgit_reference_name_to_oid(&tag_oid, repo, tagname) < 0) {
            qgit_reference_free(branch);
            qgit_reference_free(tag);
            return -1;
        }

        if (found) {
            if (qgit_oid_cmp(&branch_oid, &tag_oid) !=
                0) /* ambiguous reference */
            {
                qgit_seterror(QGITERR_AMBIGUOUS);
                qgit_reference_free(branch);
                qgit_reference_free(tag);
                return -1;
            }
        }
        qgit_oid_cpy(out, &tag_oid);
        found = 1;
    }

    if (found) {
        qgit_reference_free(branch);
        qgit_reference_free(tag);
        return 0;
    }

    size_t baselen = strlen(base);
    qgit_object *object;
    qgit_oid abbrev_oid;

    if (baselen < QGIT_OID_MINPREFIXLEN) /* too short */
        return -1;

    if (qgit_oid_fromstrn(&abbrev_oid, base, baselen) < 0)
        return -1;
    if (qgit_object_lookup_prefix(&object, repo, &abbrev_oid, baselen,
                                  QGIT_OBJ_ANY) < 0) /* abbreviated SHA-1 */
        return -1;

    qgit_oid_cpy(out, qgit_object_id(object));
    qgit_object_free(object);
    return 0;
}

int qgit_revparse_single(qgit_object **out, qgit_repository *repo,
                         const char *spec)
{
    assert(out && repo && spec);

    char *base;
    enum revision_type type;
    qgit_obj_type obj_type;
    qgit_oid oid;

    if (revparse(spec, &type, &obj_type, &base) < 0)
        return -1;

    if (resolve_oid(&oid, repo, base) < 0) {
        free(base);
        return -1;
    }

    free(base);

    qgit_object *object;

    if (qgit_object_lookup(&object, repo, &oid, QGIT_OBJ_ANY) < 0)
        return -1;

    if (type == REV_PEEL ||
        (type == REV_RESOLVE && qgit_object_type(object) == QGIT_OBJ_TAG)) {
        int ret = qgit_object_peel(out, object,
                                   type == REV_PEEL ? obj_type : QGIT_OBJ_ANY);
        qgit_object_free(object);
        return ret;
    }

    *out = object;

    return 0;
}