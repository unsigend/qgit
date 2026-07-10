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

#include <errno.h>
#include <libqgit/db/odb.h>
#include <libqgit/object/object.h>
#include <libqgit/object/signature.h>
#include <libqgit/object/tag.h>
#include <libqgit/repo/refs.h>
#include <libqgit/repo/repository.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

static int tag_fmt(void **buf, size_t *buflen, const qgit_oid *target_oid,
                   qgit_obj_type type, const char *tag_name,
                   const qgit_signature *tagger, const char *message)
{
    int n = 0;
    size_t len = 0;
    char hex[QGIT_OID_HEXSZ + 1];

    qgit_oid_fmt(hex, target_oid);
    hex[QGIT_OID_HEXSZ] = '\0';

    if ((n = snprintf(
             NULL, 0,
             "object %s\ntype %s\ntag %s\ntagger %s <%s> %ld %c%.2d%.2d\n", hex,
             qgit_object_type2string(type), tag_name, tagger->name,
             tagger->email, tagger->when.time,
             (tagger->when.offset >= 0 ? '+' : '-'),
             abs(tagger->when.offset / 60), abs(tagger->when.offset % 60))) < 0)
        return -1;
    len += n;

    if ((n = snprintf(NULL, 0, "\n%s%s", message ? message : "",
                      message ? "\n" : "")) < 0)
        return -1;
    len += n;

    char *p = malloc(len + 1);
    if (!p)
        return -1;
    *buf = p;
    *buflen = len;

    if ((n = snprintf(
             p, len + 1,
             "object %s\ntype %s\ntag %s\ntagger %s <%s> %ld %c%.2d%.2d\n", hex,
             qgit_object_type2string(type), tag_name, tagger->name,
             tagger->email, tagger->when.time,
             (tagger->when.offset >= 0 ? '+' : '-'),
             abs(tagger->when.offset / 60), abs(tagger->when.offset % 60))) <
        0) {
        free(*buf);
        return -1;
    }
    len -= n;
    p += n;

    if ((n = snprintf(p, len + 1, "\n%s%s", message ? message : "",
                      message ? "\n" : "")) < 0) {
        free(*buf);
        return -1;
    }

    return 0;
}

int qgit_tag_create(qgit_oid *oid, qgit_repository *repo, const char *tag_name,
                    const qgit_oid *target, const qgit_signature *tagger,
                    const char *message, int force)
{

    void *buf = NULL;
    size_t buflen = 0;
    qgit_object *object;
    char refname[PATH_MAX];
    qgit_reference *ref;

    if (qgit_object_lookup(&object, repo, target, QGIT_OBJ_ANY) < 0)
        return -1;

    if (tag_fmt(&buf, &buflen, target, qgit_object_type(object), tag_name,
                tagger, message) < 0) {
        qgit_object_free(object);
        return -1;
    }

    if (qgit_odb_write(oid, qgit_repository_odb(repo), buf, buflen,
                       QGIT_OBJ_TAG) < 0) {
        free(buf);
        qgit_object_free(object);
        return -1;
    }

    free(buf);
    qgit_object_free(object);

    /* if partial create occurs, the annotated tag will be orphaned and the
       lightweight tag will not be created. */

    if (snprintf(refname, PATH_MAX, "refs/tags/%s", tag_name) >= PATH_MAX) {
        errno = ENAMETOOLONG;
        return -1;
    }

    if (qgit_reference_create_oid(&ref, repo, refname, oid, force) < 0)
        return -1;

    qgit_reference_free(ref);
    return 0;
}
