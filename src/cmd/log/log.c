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

#include "log.h"

#include <libqgit/repo/repository.h>
#include <libqgit/revparse.h>
#include <libqgit/revwalk.h>
#include <limits.h>

int cmd_log(int argc, char **argv)
{
    struct argparse parser;

    if (argparse_init(&parser, options, &desc) < 0)
        die("%s", argparse_strerror(&parser));
    if (argparse_parse(&parser, argc, argv) < 0)
        die("%s", argparse_strerror(&parser));

    const char *spec = DEFAULT_TARGET;

    if (argparse_getremargc(&parser) > 0)
        spec = argparse_getremargv(&parser)[0];
    if (argparse_getremargc(&parser) > 1)
        die("too many arguments");

    if (flags.max_count < UNLIMIT)
        die("invalid value for -n: %d", flags.max_count);

    char repo_path[PATH_MAX];
    qgit_repository *repo;
    qgit_object *object;
    qgit_revwalk *walker;

    if (qgit_repository_discover(repo_path, PATH_MAX, ".") < 0)
        die_errno();
    if (qgit_repository_open(&repo, repo_path) < 0)
        die_errno();

    if (qgit_revparse_single(&object, repo, spec) <
        0) /* resolve spec to object */
        die_errno();

    if (qgit_object_type(object) !=
        QGIT_OBJ_COMMIT) /* best effort to get a commit object */
    {
        qgit_object *peeled;
        if (qgit_object_peel(&peeled, object, QGIT_OBJ_COMMIT) < 0)
            die_errno();
        qgit_object_free(object);
        object = peeled;
    }

    /* start using revwalk to walk through the commits graph */
    if (qgit_revwalk_new(&walker, repo) < 0)
        die_errno();
    qgit_revwalk_set_mode(walker, flags.first_parent
                                      ? QGIT_REVWALK_MODE_FIRST_PARENT
                                      : QGIT_REVWALK_MODE_ALL);
    qgit_revwalk_set_sort(walker, QGIT_REVWALK_SORT_TIME);

    if (qgit_revwalk_push(walker, qgit_object_id(object)) < 0)
        die_errno();

    while (flags.max_count == UNLIMIT || flags.max_count) {
        qgit_oid oid;
        qgit_commit *commit;
        int ret = qgit_revwalk_next(&oid, walker);
        if (ret == 1) /* no more commits */
            break;
        if (ret == -1)
            die_errno();
        if (qgit_commit_lookup(&commit, repo, &oid) < 0)
            die_errno();
        print_commit(commit);
        qgit_commit_free(commit);
        if (flags.max_count != UNLIMIT)
            flags.max_count--;
    }

    /* free resources */
    qgit_object_free(object);
    qgit_revwalk_free(walker);
    qgit_repository_free(repo);

    argparse_fini(&parser);
    return 0;
}
