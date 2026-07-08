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

#include "ls-tree.h"
#include "libqgit/object/tree.h"
#include "libqgit/types.h"

#include <libqgit/repo/repository.h>
#include <limits.h>

int cmd_ls_tree(int argc, char **argv)
{
    struct argparse parser;

    if (argparse_init(&parser, options, &desc) < 0)
        die("%s", argparse_strerror(&parser));
    if (argparse_parse(&parser, argc, argv) < 0)
        die("%s", argparse_strerror(&parser));

    if (argparse_getremargc(&parser) != 1)
        die("<tree-ish> is required");

    qgit_repository *repo;
    char repo_path[PATH_MAX];
    const char *tree_ish = argparse_getremargv(&parser)[0];
    qgit_oid oid;
    qgit_tree *tree;

    if (qgit_repository_discover(repo_path, PATH_MAX, ".") < 0)
        die_errno();

    if (qgit_repository_open(&repo, repo_path) < 0)
        die_errno();

    /* TODO: implement revision parsing */
    if (qgit_oid_fromstr(&oid, tree_ish) < 0)
        die_errno();

    if (qgit_tree_lookup(&tree, repo, &oid) < 0)
        die_errno();

    if (qgit_tree_walk(tree, pretty_print_tree_entry, QGIT_TREEWALK_PRE,
                       &flags) < 0)
        die_errno();

    qgit_tree_free(tree);
    qgit_repository_free(repo);
    argparse_fini(&parser);
    return 0;
}
