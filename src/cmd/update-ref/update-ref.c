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

#include "update-ref.h"

#include <die.h>
#include <errno.h>
#include <libqgit/object/object.h>
#include <libqgit/repo/refs.h>
#include <libqgit/repo/repository.h>
#include <libqgit/revparse.h>
#include <limits.h>
#include <string.h>

int cmd_update_ref(int argc, char **argv)
{
    struct argparse parser;

    if (argparse_init(&parser, options, &desc) < 0)
        die("%s", argparse_strerror(&parser));
    if (argparse_parse(&parser, argc, argv) < 0)
        die("%s", argparse_strerror(&parser));

    char repo_path[PATH_MAX];
    qgit_repository *repo;
    const char *refname;
    int remargc = argparse_getremargc(&parser);

    if (qgit_repository_discover(repo_path, PATH_MAX, ".") < 0)
        die_errno();
    if (qgit_repository_open(&repo, repo_path) < 0)
        die_errno();

    if (flags.delete) /* delete mode */
    {
        if (remargc > 2)
            die("too many arguments");
        if (remargc < 1)
            die("<ref> is required");

        refname = argparse_getremargv(&parser)[0];
        const char *old_oid =
            remargc == 2 ? argparse_getremargv(&parser)[1] : NULL;
        qgit_reference *ref;

        if (qgit_reference_lookup(&ref, repo, refname) < 0)
            die_errno();

        if (!flags.no_deref && qgit_reference_type(ref) == QGIT_REF_SYMBOLIC) {
            qgit_reference *deref;

            if (qgit_reference_resolve(&deref, ref) < 0)
                die_errno();
            qgit_reference_free(ref);
            ref = deref;
        }

        if (old_oid && strcmp(old_oid, ALL_ZERO_OID)) /* needs verify */
        {
            qgit_object *expected;
            char hex_expect[QGIT_OID_HEXSZ + 1];
            char hex_actual[QGIT_OID_HEXSZ + 1];

            if (qgit_revparse_single(&expected, repo, old_oid) < 0)
                die_errno();

            if (qgit_reference_type(ref) != QGIT_REF_DIRECT)
                die("'%s' is not a direct reference", refname);

            qgit_oid_fmt(hex_expect, qgit_object_id(expected));
            qgit_oid_fmt(hex_actual, qgit_reference_oid(ref));
            hex_expect[QGIT_OID_HEXSZ] = '\0';
            hex_actual[QGIT_OID_HEXSZ] = '\0';

            if (qgit_oid_cmp(qgit_reference_oid(ref),
                             qgit_object_id(expected)) != 0)
                die("can't update ref: '%s' which is %s but expected %s",
                    refname, hex_actual, hex_expect);

            qgit_object_free(expected);
        }

        if (qgit_reference_delete(ref) < 0)
            die_errno();

    } else /* update mode */
    {
        if (remargc > 3)
            die("too many arguments");
        if (remargc < 1)
            die("<ref> is required");
        if (remargc < 2)
            die("<new-oid> is required");

        refname = argparse_getremargv(&parser)[0];
        const char *new_oid = argparse_getremargv(&parser)[1];
        const char *old_oid =
            remargc == 3 ? argparse_getremargv(&parser)[2] : NULL;
        qgit_reference *ref = NULL;
        qgit_object *object;
        int ref_exists;

        if (qgit_revparse_single(&object, repo, new_oid) < 0)
            die_errno();

        ref_exists = qgit_reference_lookup(&ref, repo, refname) == 0;
        if (!ref_exists && errno != ENOENT)
            die_errno();

        if (old_oid && !strcmp(old_oid, ALL_ZERO_OID)) {
            if (ref_exists)
                die("cannot lock ref '%s': reference already exists", refname);
        } else if (old_oid) {
            if (!ref_exists)
                die("cannot lock ref '%s': unable to resolve reference",
                    refname);
        }

        if (!ref_exists) /* create new ref */
        {
            if (qgit_reference_create_oid(&ref, repo, refname,
                                          qgit_object_id(object), 0) < 0)
                die_errno();
            qgit_reference_free(ref);
            qgit_object_free(object);
            goto done;
        }

        if (!flags.no_deref && qgit_reference_type(ref) == QGIT_REF_SYMBOLIC) {
            qgit_reference *deref;

            if (qgit_reference_resolve(&deref, ref) < 0)
                die_errno();
            qgit_reference_free(ref);
            ref = deref;
        }

        if (old_oid && strcmp(old_oid, ALL_ZERO_OID)) /* needs verify */
        {
            qgit_object *expected;
            char hex_expect[QGIT_OID_HEXSZ + 1];
            char hex_actual[QGIT_OID_HEXSZ + 1];

            if (qgit_revparse_single(&expected, repo, old_oid) < 0)
                die_errno();

            if (qgit_reference_type(ref) != QGIT_REF_DIRECT)
                die("'%s' is not a direct reference", refname);

            qgit_oid_fmt(hex_expect, qgit_object_id(expected));
            qgit_oid_fmt(hex_actual, qgit_reference_oid(ref));
            hex_expect[QGIT_OID_HEXSZ] = '\0';
            hex_actual[QGIT_OID_HEXSZ] = '\0';

            if (qgit_oid_cmp(qgit_reference_oid(ref),
                             qgit_object_id(expected)) != 0)
                die("can't update ref: '%s' which is %s but expected %s",
                    refname, hex_actual, hex_expect);

            qgit_object_free(expected);
        }

        if (flags.no_deref && qgit_reference_type(ref) == QGIT_REF_SYMBOLIC) {
            qgit_reference_free(ref);
            if (qgit_reference_create_oid(&ref, repo, refname,
                                          qgit_object_id(object), 1) < 0)
                die_errno();
        } else {
            if (qgit_reference_type(ref) != QGIT_REF_DIRECT)
                die("'%s' is not a direct reference", refname);
            if (qgit_reference_set_oid(ref, qgit_object_id(object)) < 0)
                die_errno();
        }

        qgit_reference_free(ref);
        qgit_object_free(object);
    }

done:
    qgit_repository_free(repo);
    argparse_fini(&parser);
    return 0;
}