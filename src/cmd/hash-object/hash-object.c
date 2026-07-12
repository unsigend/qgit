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

#include "hash-object.h"

#include <die.h>
#include <fileutil.h>
#include <libqgit/db/odb.h>
#include <libqgit/object/object.h>
#include <libqgit/repo/repository.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

int cmd_hash_object(int argc, char **argv)
{
    struct argparse parser;

    if (argparse_init(&parser, options, &desc) < 0)
        die("%s", argparse_strerror(&parser));
    if (argparse_parse(&parser, argc, argv) < 0)
        die("%s", argparse_strerror(&parser));

    qgit_obj_type type = qgit_object_string2type(flags.type);
    if (type == QGIT_OBJ_BAD)
        die_errno();

    if (flags.write) /* write objects to the object database */
    {
        char repo_path[PATH_MAX];
        qgit_repository *repo;
        qgit_oid oid;
        char hex[QGIT_OID_HEXSZ + 1];

        if (qgit_repository_discover(repo_path, PATH_MAX, ".") < 0)
            die_errno();

        if (qgit_repository_open(&repo, repo_path) < 0)
            die_errno();

        for (size_t i = 0; i < argparse_getremargc(&parser); i++) {
            const char *path = argparse_getremargv(&parser)[i];
            void *buf;
            size_t len;

            if (read_file(path, &buf, &len) < 0)
                die_errno();

            if (qgit_odb_write(&oid, qgit_repository_odb(repo), buf, len,
                               type) < 0) {
                free(buf);
                die_errno();
            }

            free(buf);

            qgit_oid_fmt(hex, &oid);
            hex[QGIT_OID_HEXSZ] = '\0';
            printf("%s\n", hex);
        }

        qgit_repository_free(repo);

    } else /* compute hash only */
    {
        qgit_oid oid;
        char hex[QGIT_OID_HEXSZ + 1];

        for (size_t i = 0; i < argparse_getremargc(&parser); i++) {
            const char *path = argparse_getremargv(&parser)[i];

            if (qgit_odb_hashfile(&oid, path, type) < 0)
                die_errno();

            qgit_oid_fmt(hex, &oid);
            hex[QGIT_OID_HEXSZ] = '\0';
            printf("%s\n", hex);
        }
    }

    argparse_fini(&parser);
    return 0;
}
