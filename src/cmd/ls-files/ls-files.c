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

#include "ls-files.h"
#include "collection/string.h"

#include <die.h>
#include <libqgit/repo/index.h>
#include <libqgit/repo/repository.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

int cmd_ls_files(int argc, char **argv)
{
    struct argparse parser;

    if (argparse_init(&parser, options, &desc) < 0)
        die("%s", argparse_strerror(&parser));
    if (argparse_parse(&parser, argc, argv) < 0)
        die("%s", argparse_strerror(&parser));

    char repo_path[PATH_MAX];
    qgit_repository *repo;
    qgit_index *index;
    const char *prefix = NULL;

    if (qgit_repository_discover(repo_path, PATH_MAX, ".") < 0)
        die_errno();
    if (qgit_repository_open(&repo, repo_path) < 0)
        die_errno();

    if (argparse_getremargc(&parser) > 0)
        prefix = argparse_getremargv(&parser)[0];
    if (argparse_getremargc(&parser) > 1)
        die("too many arguments");

    index = qgit_repository_index(repo);

    for (unsigned int i = 0; i < qgit_index_entrycount(index); i++) {
        qgit_index_entry *entry = qgit_index_get(index, i);
        if (prefix) {
            size_t prefixlen = strlen(prefix);
            if (!(str_startswith(entry->path, prefix) &&
                  (entry->path[prefixlen] == '/' ||
                   entry->path[prefixlen] == '\0')))
                continue;
        }
        if (flags.stage) {
            char hex[QGIT_OID_HEXSZ + 1];
            qgit_oid_fmt(hex, &entry->oid);
            hex[QGIT_OID_HEXSZ] = '\0';

            printf("%.6o %s %.1d\t", entry->mode, hex,
                   qgit_index_entry_stage(entry));
        }
        printf("%s\n", entry->path);
    }

    qgit_repository_free(repo);
    argparse_fini(&parser);
    return 0;
}
