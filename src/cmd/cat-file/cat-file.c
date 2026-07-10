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

#include "cat-file.h"

#include <libqgit/db/odb.h>
#include <libqgit/oid.h>
#include <libqgit/repo/repository.h>
#include <libqgit/revparse.h>
#include <limits.h>
#include <stdio.h>

int cmd_cat_file(int argc, char **argv)
{
    struct argparse parser;

    if (argparse_init(&parser, options, &desc) < 0)
        die("%s", argparse_strerror(&parser));
    if (argparse_parse(&parser, argc, argv) < 0)
        die("%s", argparse_strerror(&parser));

    mutex_check();

    int rawmode = !flags.pretty && !flags.print_type && !flags.print_size;
    qgit_repository *repo;
    char repo_path[PATH_MAX];
    qgit_object *object;

    if (qgit_repository_discover(repo_path, PATH_MAX, ".") < 0)
        die_errno();

    if (qgit_repository_open(&repo, repo_path) < 0)
        die_errno();

    if (rawmode) /* raw mode */
    {
        if (argparse_getremargc(&parser) != 2)
            die("<type> <object> mode needs two arguments");

        const char *type = argparse_getremargv(&parser)[0];
        const char *object_name = argparse_getremargv(&parser)[1];
        qgit_odb *odb;
        qgit_odb_object *odb_obj;
        qgit_obj_type obj_type;

        if (qgit_revparse_single(&object, repo, object_name) < 0)
            die_errno();

        if ((odb = qgit_repository_odb(repo)) == NULL)
            die_errno();

        if (qgit_odb_read(&odb_obj, odb, qgit_object_id(object)) < 0)
            die_errno();

        if ((obj_type = qgit_object_string2type(type)) == QGIT_OBJ_BAD)
            die_errno();

        if (obj_type != qgit_odb_object_type(odb_obj))
            die("expect object type %s, but actual is %s", type,
                qgit_object_type2string(qgit_odb_object_type(odb_obj)));

        fwrite(qgit_odb_object_data(odb_obj), 1, qgit_odb_object_size(odb_obj),
               stdout);
        fflush(stdout);

        qgit_odb_object_free(odb_obj);
    } else /* auto mode */
    {
        if (argparse_getremargc(&parser) != 1)
            die("<object> is missing");

        const char *object_name = argparse_getremargv(&parser)[0];

        if (qgit_revparse_single(&object, repo, object_name) < 0)
            die_errno();

        if (flags.print_size ||
            flags.print_type) /* use odb directly to read the raw payload */
        {
            qgit_odb *odb;
            qgit_odb_object *odb_obj;

            if ((odb = qgit_repository_odb(repo)) == NULL)
                die_errno();

            if (qgit_odb_read(&odb_obj, odb, qgit_object_id(object)) < 0)
                die_errno();

            if (flags.print_size)
                fprintf(stdout, "%zu\n", qgit_odb_object_size(odb_obj));
            else if (flags.print_type)
                fprintf(stdout, "%s\n",
                        qgit_object_type2string(qgit_odb_object_type(odb_obj)));

            qgit_odb_object_free(odb_obj);
        } else
            pretty_print_object(object);
    }

    qgit_object_free(object);
    qgit_repository_free(repo);
    argparse_fini(&parser);
    return 0;
}
