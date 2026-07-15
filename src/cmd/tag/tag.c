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

#include "tag.h"

#include <credentials.h>
#include <libqgit/object/object.h>
#include <libqgit/object/tag.h>
#include <libqgit/repo/refs.h>
#include <libqgit/repo/repository.h>
#include <libqgit/revparse.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>

int cmd_tag(int argc, char **argv)
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
            die("<tagname> is required");
        const char *tagname = argparse_getremargv(&parser)[0];

        if (qgit_tag_delete(repo, tagname) < 0)
            die_errno();

    } else if (flags.list || remargc == 0) /* list mode */
    {
        struct vector *tags;
        if (qgit_tag_list(&tags, repo) < 0)
            die_errno();

        for (size_t i = 0; i < vec_size(tags); i++) {
            const char *tag = *(char **)vec_at(tags, i);
            printf("%s\n", tag);
        }

        vec_free(tags);
    } else /* create mode */
    {
        if (remargc < 1)
            die("<tagname> is required");
        if (remargc > 2)
            die("too many arguments");
        const char *tagname = argparse_getremargv(&parser)[0];
        const char *spec =
            remargc == 2 ? argparse_getremargv(&parser)[1] : DEFAULT_TARGET;

        qgit_object *object;

        if (qgit_revparse_single(&object, repo, spec) < 0)
            die_errno();

        if (flags.annotate) /* annotated tag */
        {
            qgit_signature *signature;
            qgit_oid oid;
            if (credentials_get(&signature, repo) < 0)
                die_errno();

            if (qgit_tag_create(&oid, repo, tagname, qgit_object_id(object),
                                signature, flags.message, flags.force) < 0)
                die_errno();

            qgit_signature_free(signature);

        } else /* lightweight tag */
        {
            if (flags.message)
                die("lightweight tags cannot have a message");
            qgit_oid oid;
            if (qgit_tag_create_lightweight(&oid, repo, tagname,
                                            qgit_object_id(object),
                                            flags.force) < 0)
                die_errno();
        }

        qgit_object_free(object);
    }

    qgit_repository_free(repo);
    argparse_fini(&parser);
    return 0;
}
