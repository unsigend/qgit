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

#include "add.h"

#include <libqgit/repo/repository.h>

int cmd_add(int argc, char **argv)
{
    struct argparse parser;

    if (argparse_init(&parser, options, &desc) < 0)
        die("%s", argparse_strerror(&parser));
    if (argparse_parse(&parser, argc, argv) < 0)
        die("%s", argparse_strerror(&parser));

    int remargc = argparse_getremargc(&parser);
    char repo_path[PATH_MAX];
    qgit_repository *repo;
    qgit_index *index;

    if (remargc == 0)
        die("nothing specified, nothing added.");

    if (qgit_repository_discover(repo_path, PATH_MAX, ".") < 0)
        die_errno();
    if (qgit_repository_open(&repo, repo_path) < 0)
        die_errno();

    index = qgit_repository_index(repo);

    for (int i = 0; i < remargc; i++) {
        const char *path = argparse_getremargv(&parser)[i];
        add_path(index, path, qgit_repository_workdir(repo));
    }

    if (qgit_index_write(index) < 0)
        die_errno();

    qgit_repository_free(repo);
    argparse_fini(&parser);
    return 0;
}
