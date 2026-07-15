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

#include "commit.h"

#include <credentials.h>
#include <die.h>
#include <errno.h>
#include <libqgit/repo/refs.h>
#include <libqgit/repo/repository.h>
#include <limits.h>

int cmd_commit(int argc, char **argv)
{
    struct argparse parser;
    char repo_path[PATH_MAX];
    qgit_repository *repo;
    qgit_signature *signature;
    qgit_index *index;
    qgit_reference *head, *resolved = NULL;
    qgit_commit *parent = NULL;
    const qgit_commit *parents[1] = {NULL};
    qgit_oid tree_oid, commit_oid;
    qgit_tree *tree;
    qgit_commit *commit;

    if (argparse_init(&parser, options, &desc) < 0)
        die("%s", argparse_strerror(&parser));
    if (argparse_parse(&parser, argc, argv) < 0)
        die("%s", argparse_strerror(&parser));
    if (!flags.message)
        die("message is required via -m");

    if (qgit_repository_discover(repo_path, PATH_MAX, ".") < 0)
        die_errno();
    if (qgit_repository_open(&repo, repo_path) < 0)
        die_errno();
    if (credentials_get(&signature, repo) < 0)
        die_errno();
    if (!(index = qgit_repository_index(repo)))
        die_errno();

    /* parent: HEAD tip, or none when the branch is unborn */
    if (qgit_repository_head(&head, repo) < 0)
        die_errno();
    errno = 0;
    if (qgit_reference_resolve(&resolved, head) < 0 && errno != ENOENT)
        die_errno();
    qgit_reference_free(head);

    if (resolved) {
        if (qgit_commit_lookup(&parent, repo, qgit_reference_oid(resolved)) < 0)
            die_errno();
        qgit_reference_free(resolved);
        resolved = NULL;
        parents[0] = parent; /* one parent only in current scope */
    }

    if (qgit_tree_create_fromindex(&tree_oid, index) < 0)
        die_errno();
    if (qgit_tree_lookup(&tree, repo, &tree_oid) < 0)
        die_errno();
    if (qgit_commit_create(&commit_oid, repo, "HEAD", signature, signature,
                           flags.message, tree, parent ? 1 : 0, parents) < 0)
        die_errno();

    /* summary needs the resolved tip after HEAD was updated */
    if (qgit_commit_lookup(&commit, repo, &commit_oid) < 0)
        die_errno();
    if (qgit_repository_head(&head, repo) < 0)
        die_errno();
    if (qgit_reference_resolve(&resolved, head) < 0)
        die_errno();
    print_commit_result(commit, resolved);

    qgit_reference_free(resolved);
    qgit_reference_free(head);

    qgit_commit_free(commit);
    qgit_commit_free(parent);

    qgit_tree_free(tree);
    qgit_signature_free(signature);
    qgit_repository_free(repo);
    argparse_fini(&parser);
    return 0;
}
