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

#ifndef CMD_BRANCH_H
#define CMD_BRANCH_H

#include <argparse.h>
#include <die.h>

#define DEFAULT_START_POINT "HEAD"

#define ASCII_COLOR_GREEN "\033[32m"
#define ASCII_COLOR_RESET "\033[0m"

struct cmd_branch_flags {
    int list;   /* -l --list */
    int delete; /* -d --delete */
    int force;  /* -f --force */
};

static struct cmd_branch_flags flags = {
    .list = 0,
    .delete = 0,
    .force = 0,
};

static struct argparse_opt options[] = {
    OPT_HELP(),
    OPT_BOOL('l', "list", "List branches", &flags.list),
    OPT_BOOL('d', "delete", "Delete the branch with the given name",
             &flags.delete),
    OPT_BOOL('f', "force", "Force create or allow deleting the current branch",
             &flags.force),
    OPT_END(),
};

static const char *usages[] = {
    "qgit branch [options] [<branchname> [<start-point>]]",
    "qgit branch [options] -d <branchname>",
};

static struct argparse_desc desc = {
    .prog = "qgit branch",
    .desc = "List, create, or delete branches",
    .usages = usages,
    .nusages = sizeof(usages) / sizeof(usages[0]),
    .epilog = "With no arguments, list branches. <start-point> defaults to "
              "HEAD.",
};

static void mutex_check(void)
{
    if (flags.delete && flags.list)
        die("Cannot use -d and -l together");
}

#endif
