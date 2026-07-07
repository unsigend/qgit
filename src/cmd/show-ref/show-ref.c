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

#include "show-ref.h"

#include <die.h>
#include <libqgit/repo/refs.h>
#include <libqgit/repo/repository.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>

int cmd_show_ref(int argc, char **argv)
{
    struct argparse parser;

    if (argparse_init(&parser, options, &desc) < 0)
        die("%s", argparse_strerror(&parser));
    if (argparse_parse(&parser, argc, argv) < 0)
        die("%s", argparse_strerror(&parser));

    qgit_repository *repo;
    char hex[QGIT_OID_HEXSZ + 1];
    char repo_path[PATH_MAX];
    unsigned int flag;
    struct vector *refs;
    qgit_oid oid;

    if (qgit_repository_discover(repo_path, PATH_MAX, ".") < 0)
        die_errno();

    if (qgit_repository_open(&repo, repo_path) < 0)
        die_errno();

    if (flags.head) {
        if (qgit_reference_name_to_oid(&oid, repo, "HEAD") < 0)
            die_errno();

        qgit_oid_fmt(hex, &oid);
        hex[QGIT_OID_HEXSZ] = '\0';
        printf("%s HEAD\n", hex);
    }

    if (!flags.branches && !flags.tags)
        flag = QGIT_REF_LIST_ALL;
    else {
        flag = 0;
        if (flags.branches)
            flag |= QGIT_REF_LIST_BRANCHES;
        if (flags.tags)
            flag |= QGIT_REF_LIST_TAGS;
    }

    if (qgit_reference_list(&refs, repo, flag) < 0)
        die_errno();

    for (size_t i = 0; i < vec_size(refs); i++) {
        const char *refname = *(char **)vec_at(refs, i);

        if (qgit_reference_name_to_oid(&oid, repo, refname) < 0)
            die_errno();

        qgit_oid_fmt(hex, &oid);
        hex[QGIT_OID_HEXSZ] = '\0';
        printf("%s %s\n", hex, refname);
    }

    vec_free(refs);
    qgit_repository_free(repo);
    argparse_fini(&parser);
    return 0;
}
