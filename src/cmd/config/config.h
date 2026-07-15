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

#ifndef CMD_CONFIG_H
#define CMD_CONFIG_H

#include <argparse.h>
#include <die.h>
#include <errno.h>
#include <fileutils.h>
#include <libqgit/error.h>
#include <libqgit/repo/config.h>
#include <limits.h>

struct cmd_config_flags {
    int global; /* use the global config file */
    int local;  /* use the local config file  */
    int list;   /* list mode */
    int get;    /* get mode */
    int set;    /* set mode */
    int unset;  /* unset mode */
};

static struct cmd_config_flags flags = {
    .global = 0,
    .local = 0,
    .list = 0,
    .get = 0,
    .set = 0,
    .unset = 0,
};

static struct argparse_opt options[] = {
    OPT_HELP(),
    OPT_BOOL(0, "global", "Use the global config file only", &flags.global),
    OPT_BOOL(0, "local", "Use the local repository config file only",
             &flags.local),
    OPT_BOOL('l', "list", "List all variables for the selected scope",
             &flags.list),
    OPT_BOOL('g', "get", "Print the value of key", &flags.get),
    OPT_BOOL('s', "set", "Set key to value", &flags.set),
    OPT_BOOL('u', "unset", "Remove key", &flags.unset),
    OPT_END(),
};

static const char *usages[] = {
    "qgit config [--global | --local] --list",
    "qgit config [--global | --local] --get <key>",
    "qgit config [--global | --local] --set <key> <value>",
    "qgit config [--global | --local] --unset <key>",
};

static struct argparse_desc desc = {
    .prog = "qgit config",
    .desc = "Get and set repository or global options",
    .usages = usages,
    .nusages = sizeof(usages) / sizeof(usages[0]),
    .epilog = "keys use the format <section>.<name>",
};

static void action_check(void)
{
    if (!flags.get && !flags.set && !flags.unset && !flags.list)
        die("no action specified");

    if (flags.global && flags.local)
        die("cannot use --global and --local together");

    if ((flags.list && flags.get) || (flags.list && flags.set) ||
        (flags.list && flags.unset) || (flags.get && flags.set) ||
        (flags.get && flags.unset) || (flags.set && flags.unset))
        die("cannot use multiple actions together");
}

static void clearerrno(void)
{
    errno = 0;
    qgit_clear_error();
}

/* Safely open the global config file, if not exist, create it */
static int open_global_config_safe(qgit_config **config)
{
    char path[PATH_MAX];

    if (qgit_config_find_global(path, PATH_MAX) == -1)
        return -1;

    if (!file_exists(path)) {
        if (qgit_config_create_global(config) < 0)
            return -1;
    } else if (qgit_config_open_global(config) < 0)
        return -1;

    return 0;
}

#endif
