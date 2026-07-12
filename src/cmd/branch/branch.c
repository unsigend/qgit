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

#include "branch.h"

#include <libqgit/object/object.h>
#include <libqgit/repo/branch.h>
#include <libqgit/repo/refs.h>
#include <libqgit/repo/repository.h>
#include <libqgit/revparse.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int cmd_branch(int argc, char **argv)
{
    struct argparse parser;

    if (argparse_init(&parser, options, &desc) < 0)
        die("%s", argparse_strerror(&parser));
    if (argparse_parse(&parser, argc, argv) < 0)
        die("%s", argparse_strerror(&parser));

    mutex_check();

    int remargc = argparse_getremargc(&parser);
    char repo_path[PATH_MAX];
    qgit_repository *repo;

    if (qgit_repository_discover(repo_path, PATH_MAX, ".") < 0)
        die_errno();
    if (qgit_repository_open(&repo, repo_path) < 0)
        die_errno();

    if (flags.delete) /* delete mode */
    {
        if (remargc != 1)
            die("<branchname> is required");

        const char *branchname = argparse_getremargv(&parser)[0];
        qgit_reference *branch;
        int is_head;

        /* for the current qgit scope the branch only support local branch */
        if (qgit_branch_lookup(&branch, repo, branchname, QGIT_BRANCH_LOCAL) <
            0)
            die_errno();

        is_head = qgit_branch_is_head(branch);
        if (is_head == -1)
            die_errno();

        if (is_head && !flags.force)
            die("Cannot delete the current branch without -f");

        if (qgit_branch_delete(repo, branchname, QGIT_BRANCH_LOCAL) < 0)
            die_errno();

        qgit_reference_free(branch);
    } else if (flags.list || remargc == 0) /* list mode */
    {
        struct vector *branches;
        int istty = isatty(fileno(stdout));
        qgit_reference *head;

        if (qgit_branch_list(&branches, repo, QGIT_BRANCH_LOCAL,
                             QGIT_BRANCH_SORT_NAME) < 0)
            die_errno();

        if (qgit_repository_head(&head, repo) < 0)
            die_errno();

        for (size_t i = 0; i < vec_size(branches); i++) {
            const char *branch = *(char **)vec_at(branches, i);
            if (qgit_reference_type(head) == QGIT_REF_SYMBOLIC &&
                strcmp(qgit_reference_target(head) + strlen("refs/heads/"),
                       branch) == 0) /* current head branch */
            {
                printf("* %s%s%s\n", istty ? ASCII_COLOR_GREEN : "", branch,
                       istty ? ASCII_COLOR_RESET : "");
            } else {
                printf("  %s\n", branch);
            }
        }

        vec_free(branches);
        qgit_reference_free(head);

    } else /* create mode */
    {
        if (remargc < 1)
            die("<branchname> is required");
        if (remargc > 2)
            die("too many arguments");
        const char *branchname = argparse_getremargv(&parser)[0];
        const char *spec = remargc == 2 ? argparse_getremargv(&parser)[1]
                                        : DEFAULT_START_POINT;

        qgit_object *object;
        qgit_reference *ref;

        if (qgit_revparse_single(&object, repo, spec) < 0)
            die_errno();

        if (qgit_object_type(object) !=
            QGIT_OBJ_COMMIT) /* auto peel to a commit */
        {
            qgit_object *peeled;
            if (qgit_object_peel(&peeled, object, QGIT_OBJ_COMMIT) < 0)
                die_errno();
            qgit_object_free(object);
            object = peeled;
        }

        if (qgit_branch_create(&ref, repo, branchname, qgit_object_id(object),
                               flags.force) < 0)
            die_errno();

        qgit_reference_free(ref);
        qgit_object_free(object);
    }

    qgit_repository_free(repo);
    argparse_fini(&parser);
    return 0;
}
