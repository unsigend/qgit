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

#include "rev-parse.h"

#include <die.h>
#include <libqgit/object/object.h>
#include <libqgit/repo/repository.h>
#include <libqgit/revparse.h>
#include <limits.h>
#include <stdio.h>

int cmd_rev_parse(int argc, char **argv)
{
    struct argparse parser;
    if (argparse_init(&parser, options, &desc) < 0)
        die("%s", argparse_strerror(&parser));
    if (argparse_parse(&parser, argc, argv) < 0)
        die("%s", argparse_strerror(&parser));

    char repo_path[PATH_MAX];
    qgit_repository *repo;

    if (qgit_repository_discover(repo_path, PATH_MAX, ".") < 0)
        die_errno();
    if (qgit_repository_open(&repo, repo_path) < 0)
        die_errno();

    for (size_t i = 0; i < argparse_getremargc(&parser); i++) {
        const char *spec = argparse_getremargv(&parser)[i];
        char hex[QGIT_OID_HEXSZ + 1];
        qgit_object *object;

        if (qgit_revparse_single(&object, repo, spec) < 0)
            die_errno();

        qgit_oid_fmt(hex, qgit_object_id(object));
        hex[QGIT_OID_HEXSZ] = '\0';
        printf("%s\n", hex);
        qgit_object_free(object);
    }

    qgit_repository_free(repo);
    argparse_fini(&parser);
    return 0;
}
