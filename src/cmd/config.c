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

#include <argparse.h>
#include <die.h>
#include <errno.h>
#include <libqgit.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void action_check(int list, int get, int set, int unset)
{
    if ((list && (get || set || unset)) || (get && (set || unset)) ||
        (set && unset))
        die("cannot use multiple actions at once");
    if (!list && !get && !set && !unset)
        die("no action specified");
}

static void scope_check(int g, int l)
{
    if (g && l)
        die("cannot use --global and --local together");
}

/*
  qgit config behaviour:
    --list: if --global or --local is specified, list the global or local
    config file, if auto, list global first then local config.

    --get: if --global or --local is specified, get the value of the key
    from the global or local config file, if auto, get the value from local
    config local config first, then from global config if not found.

    --set:  if --global or --local is specified, set the value of the key in
    the global or local config file, if auto, set the value in local config.

    --unset: if --global or --local is specified, unset the value of the key
    in the global or local config file, if auto, unset the value in local
    config.
*/

int cmd_config(int argc, char **argv)
{
    int scope_g = 0, scope_l = 0, scope_a = 0;
    int list = 0, get = 0, set = 0, unset = 0;
    qgit_repository *repo = NULL;
    qgit_config *global_cfg = NULL, *local_cfg = NULL;
    char repo_path[PATH_MAX];
    int ret = 0;

    struct argparse parser;
    struct argparse_opt opts[] = {
        OPT_HELP(),
        OPT_GROUP("Scope"),
        OPT_BOOL(0, "global", "use global config file", &scope_g),
        OPT_BOOL(0, "local", "use local config file", &scope_l),
        OPT_GROUP_END(),
        OPT_GROUP("Action"),
        OPT_BOOL('l', "list", "list all options", &list),
        OPT_BOOL('g', "get", "get the value of a key", &get),
        OPT_BOOL('s', "set", "set the value of a key", &set),
        OPT_BOOL('u', "unset", "unset the value of a key", &unset),
        OPT_GROUP_END(),
        OPT_END(),
    };

    static const char *usages[] = {
        "qgit config [<scope>] --list",
        "qgit config [<scope>] --get <key>",
        "qgit config [<scope>] --set <key> <value>",
        "qgit config [<scope>] --unset <key>",
    };

    struct argparse_desc desc = {
        .prog = "qgit config",
        .desc = "Get and set repository or global options",
        .usages = usages,
        .nusages = sizeof(usages) / sizeof(usages[0]),
        .epilog = "key format: <section>.<name>",
    };

    if (argparse_init(&parser, opts, &desc) == -1)
        die("%s", argparse_strerror(&parser));
    if (argparse_parse(&parser, argc, argv) == -1)
        die("%s", argparse_strerror(&parser));

    action_check(list, get, set, unset);
    scope_check(scope_g, scope_l);
    scope_a = !scope_g && !scope_l;

    /* discover repository best effort not required for --global */
    if (!scope_g) {
        if (qgit_repository_discover(repo_path, PATH_MAX, ".") == 0)
            qgit_repository_open(&repo, repo_path);
    }

    if (scope_l && !repo)
        die("not inside a qgit repository");

    /* open configs global may not exist */
    if (scope_g || scope_a)
        qgit_config_open_global(&global_cfg);

    if ((scope_l || scope_a) && repo)
        qgit_repository_config(&local_cfg, repo);

    if (list) /* list mode */
    {
        if (scope_g && global_cfg)
            qgit_config_fprint(global_cfg, stdout);
        else if (scope_l && local_cfg)
            qgit_config_fprint(local_cfg, stdout);
        else {
            if (global_cfg)
                qgit_config_fprint(global_cfg, stdout);
            if (local_cfg)
                qgit_config_fprint(local_cfg, stdout);
        }
    } else if (get) /* get mode */
    {
        if (argparse_getremargc(&parser) < 1)
            die("--get requires a key");

        const char *key = argparse_getremargv(&parser)[0];
        const char *val;

        if (scope_g) /* global scope */
        {
            if (!global_cfg ||
                qgit_config_get_string(&val, global_cfg, key) == -1) {
                if (qgit_geterrno() == QGITERR_INVKEY)
                    die_errno();
                ret = 1;
            } else
                printf("%s\n", val);
        } else if (scope_l) /* local scope */
        {
            if (qgit_config_get_string(&val, local_cfg, key) == -1) {
                if (qgit_geterrno() == QGITERR_INVKEY)
                    die_errno();
                ret = 1;
            } else
                printf("%s\n", val);
        } else /* auto scope */
        {
            int found = 0;
            if (local_cfg) {
                if (qgit_config_get_string(&val, local_cfg, key) != -1) {
                    printf("%s\n", val);
                    found = 1;
                }
            }
            if (!found && global_cfg) {
                if (qgit_config_get_string(&val, global_cfg, key) != -1) {
                    printf("%s\n", val);
                    found = 1;
                }
            }
            if (!found) {
                if (qgit_geterrno() == QGITERR_INVKEY)
                    die_errno();
                ret = 1;
            }
        }
    } else if (set) /* set mode */
    {
        if (argparse_getremargc(&parser) < 2)
            die("--set requires a key and a value");

        const char *key = argparse_getremargv(&parser)[0];
        const char *val = argparse_getremargv(&parser)[1];

        if (scope_g) {
            if (!global_cfg) { /* create global config if it doesn't exist */
                char global_cfg_path[PATH_MAX];
                if (qgit_config_global_path(global_cfg_path, PATH_MAX) == -1)
                    die_errno();
                if (qgit_config_create(&global_cfg, global_cfg_path) == -1)
                    die_errno();
            }
            if (qgit_config_set_string(global_cfg, key, val) == -1 ||
                qgit_config_write(global_cfg) == -1)
                die_errno();
        } else {
            if (scope_a && !repo)
                die("not inside a qgit repository");
            if (qgit_config_set_string(local_cfg, key, val) == -1 ||
                qgit_config_write(local_cfg) == -1)
                die_errno();
        }
    } else /* unset mode */
    {
        if (argparse_getremargc(&parser) < 1)
            die("--unset requires a key");
        const char *key = argparse_getremargv(&parser)[0];

        if (scope_g) {
            if (!global_cfg || qgit_config_delete(global_cfg, key) == -1) {
                if (qgit_geterrno() == QGITERR_INVKEY)
                    die_errno();
                ret = 1;
            } else if (qgit_config_write(global_cfg) == -1)
                die_errno();
        } else {
            if (scope_a && !repo)
                die("not inside a qgit repository");
            if (qgit_config_delete(local_cfg, key) == -1) {
                if (qgit_geterrno() == QGITERR_INVKEY)
                    die_errno();
                ret = 1;
            } else if (qgit_config_write(local_cfg) == -1)
                die_errno();
        }
    }

    qgit_config_free(global_cfg);
    qgit_repository_free(repo);
    argparse_fini(&parser);
    return ret;
}