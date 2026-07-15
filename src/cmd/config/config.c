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

#include "config.h"

#include <libqgit/repo/repository.h>
#include <stdlib.h>

int cmd_config(int argc, char **argv)
{
    struct argparse parser;

    if (argparse_init(&parser, options, &desc) < 0)
        die("%s", argparse_strerror(&parser));
    if (argparse_parse(&parser, argc, argv) < 0)
        die("%s", argparse_strerror(&parser));

    action_check();

    char repo_path[PATH_MAX];
    qgit_repository *repo = NULL;
    qgit_config *global_config, *local_config = NULL;
    int ret = 0;

    if (qgit_repository_discover(repo_path, PATH_MAX, ".") !=
        -1) /* best effort */
    {
        if (qgit_repository_open(&repo, repo_path) < 0)
            die_errno();

        local_config = qgit_repository_config(repo);
        if (!local_config)
            die("missing config file for repository");
    }
    clearerrno();

    if (flags.local && !repo) {
        qgit_seterror(QGITERR_REPONOTFOUND);
        die_errno();
    }

    /* safe open global config file, if doesn't exist create an empty memory
       instance, doesn't persist on disk unless --set is specified. */
    if (open_global_config_safe(&global_config) < 0)
        die_errno();

    if (flags.list) /* list mode */
    {
        if (flags.global) {
            if (qgit_config_fprint(global_config, stdout) < 0)
                die_errno();
        } else if (flags.local) {
            if (qgit_config_fprint(local_config, stdout) < 0)
                die_errno();
        } else {
            if (qgit_config_fprint(global_config, stdout) < 0)
                die_errno();
            if (local_config && qgit_config_fprint(local_config, stdout) < 0)
                die_errno();
        }
    }

    else if (flags.get) /* get mode */
    {
        if (argparse_getremargc(&parser) < 1)
            die("get mode requires a key");

        const char *key = argparse_getremargv(&parser)[0];
        const char *value = NULL;

        clearerrno();

        if (flags.global)
            qgit_config_get_string(&value, global_config, key);
        else if (flags.local)
            qgit_config_get_string(&value, local_config, key);
        else {
            if (local_config)
                qgit_config_get_string(&value, local_config,
                                       key); /* local config first */

            if (errno || qgit_error())
                die_errno();

            if (!value)
                qgit_config_get_string(&value, global_config, key);
        }

        if (errno || qgit_error())
            die_errno();

        if (value)
            printf("%s\n", value);
        else
            ret = EXIT_FAILURE;
    }

    else if (flags.set) /* set mode */
    {
        if (argparse_getremargc(&parser) < 2)
            die("set mode requires a key and a value");

        const char *key = argparse_getremargv(&parser)[0];
        const char *value = argparse_getremargv(&parser)[1];
        qgit_config *target;

        if (flags.global)
            target = global_config;
        else {
            if (!local_config) {
                qgit_seterror(QGITERR_REPONOTFOUND);
                die_errno();
            }
            target = local_config;
        }

        if (qgit_config_set_string(target, key, value) < 0)
            die_errno();
        if (qgit_config_save(target) < 0)
            die_errno();
    }

    else if (flags.unset) /* unset mode */
    {
        if (argparse_getremargc(&parser) < 1)
            die("unset mode requires a key");

        const char *key = argparse_getremargv(&parser)[0];
        qgit_config *target;

        if (flags.global)
            target = global_config;
        else {
            if (!local_config) {
                qgit_seterror(QGITERR_REPONOTFOUND);
                die_errno();
            }
            target = local_config;
        }

        clearerrno();

        if (qgit_config_delete(target, key) < 0) {
            if (errno || qgit_error())
                die_errno();
            ret = EXIT_FAILURE;
        } else if (qgit_config_save(target) < 0)
            die_errno();

    } else /* unknown action */
        die("unknown action");

    qgit_repository_free(repo);
    qgit_config_free(global_config);
    argparse_fini(&parser);
    return ret;
}
