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

#ifndef CMD_INIT_H
#define CMD_INIT_H

#include <argparse.h>

#define DEFAULT_BRANCH "main"
#define DEFAULT_PATH "."

struct cmd_init_flags {
    int quiet;          /* quiet mode */
    const char *branch; /* branch name */
    const char *path;   /* path to initialize repository */
};

static struct cmd_init_flags flags = {
    .branch = DEFAULT_BRANCH,
    .path = DEFAULT_PATH,
    .quiet = 0,
};

static struct argparse_opt options[] = {
    OPT_HELP(),
    OPT_BOOL('q', "quiet", "Suppress verbose output", &flags.quiet),
    OPT_STR('b', "initial-branch", "Specify the branch name", &flags.branch,
            OPT_REQUIRED),
    OPT_END(),
};

static const char *usages[] = {
    "qgit init [-q | --quiet] [-b <branch-name> | "
    "--initial-branch=<branch-name>] "
    "[<path>]",
};

static struct argparse_desc desc = {
    .prog = "qgit init",
    .desc = "Create an empty qgit repository or reinitialize an existing one",
    .usages = usages,
    .nusages = sizeof(usages) / sizeof(usages[0]),
    .epilog = "The default initial branch is 'main'.",
};

#endif
