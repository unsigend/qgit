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

#include "checkout.h"

#include <die.h>
#include <fileutils.h>
#include <libqgit/checkout.h>
#include <libqgit/object/object.h>
#include <libqgit/repo/repository.h>
#include <libqgit/revparse.h>
#include <limits.h>

int cmd_checkout(int argc, char **argv)
{
    struct argparse parser;
    char repo_path[PATH_MAX];
    qgit_repository *repo;
    const char *spec = NULL;
    const char *path = NULL;
    qgit_object *obj;

    if (argparse_init(&parser, options, &desc) < 0)
        die("%s", argparse_strerror(&parser));
    if (argparse_parse(&parser, argc, argv) < 0)
        die("%s", argparse_strerror(&parser));

    int remargc = argparse_getremargc(&parser);

    if (remargc < 2)
        die("<commit> and <path> are required");
    if (remargc > 2)
        die("too many arguments");

    spec = argparse_getremargv(&parser)[0];
    path = argparse_getremargv(&parser)[1];

    if (path_exists(path))
        die("'%s' already exists", path);

    if (qgit_repository_discover(repo_path, PATH_MAX, ".") < 0)
        die_errno();
    if (qgit_repository_open(&repo, repo_path) < 0)
        die_errno();

    if (qgit_revparse_single(&obj, repo, spec) < 0)
        die_errno();

    if (qgit_object_type(obj) != QGIT_OBJ_TREE) {
        qgit_object *peeled;
        if (qgit_object_peel(&peeled, obj, QGIT_OBJ_TREE) < 0)
            die_errno();
        qgit_object_free(obj);
        obj = peeled;
    }

    if (qgit_checkout_tree(repo, (qgit_tree *)obj, path) < 0)
        die_errno();

    qgit_object_free(obj);
    qgit_repository_free(repo);
    argparse_fini(&parser);
    return 0;
}
