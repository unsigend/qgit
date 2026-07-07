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

#include "init.h"

#include <die.h>
#include <errno.h>
#include <fs.h>
#include <libqgit/repo/repository.h>
#include <limits.h>
#include <stdio.h>

int cmd_init(int argc, char **argv)
{
    struct argparse parser;

    if (argparse_init(&parser, options, &desc) < 0)
        die("%s", argparse_strerror(&parser));
    if (argparse_parse(&parser, argc, argv) < 0)
        die("%s", argparse_strerror(&parser));

    if (argparse_getremargc(&parser) > 0)
        flags.path = argparse_getremargv(&parser)[0];
    if (argparse_getremargc(&parser) > 1)
        die("too many arguments");

    int reinit = 0;
    qgit_repository *repo;
    char abspath[PATH_MAX];
    char repodir[PATH_MAX];

    if (fabspath(flags.path, abspath) < 0)
        die_errno();

    if (snprintf(repodir, PATH_MAX, "%s/.qgit", abspath) >= PATH_MAX) {
        errno = ENAMETOOLONG;
        die_errno();
    }

    if (dir_exists(repodir))
        reinit = 1;

    if (qgit_repository_init(&repo, abspath, flags.branch) < 0)
        die_errno();

    if (!flags.quiet) {
        if (reinit)
            printf("Reinitialized existing repository in %s/\n",
                   qgit_repository_path(repo));
        else
            printf("Initialized empty repository in %s/\n",
                   qgit_repository_path(repo));
    }

    qgit_repository_free(repo);
    argparse_fini(&parser);
    return 0;
}
