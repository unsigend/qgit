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

#ifndef CMD_SHOW_REF_H
#define CMD_SHOW_REF_H

#include <argparse.h>

struct cmd_show_ref_flags {
    int head;     /* show HEAD reference */
    int branches; /* limit to refs/heads */
    int tags;     /* limit to refs/tags */
};

static struct cmd_show_ref_flags flags = {
    .head = 0,
    .branches = 0,
    .tags = 0,
};

struct argparse_opt options[] = {
    OPT_HELP(),
    OPT_BOOL(0, "head",
             "Show the HEAD reference, even if it would normally be "
             "filtered out",
             &flags.head),
    OPT_BOOL(0, "branches", "Limit to local branches", &flags.branches),
    OPT_BOOL(0, "tags", "Limit to local tags", &flags.tags),
    OPT_END(),
};

static const char *usages[] = {
    "qgit show-ref [--head] [--branches] [--tags]",
};

struct argparse_desc desc = {
    .prog = "qgit show-ref",
    .desc = "List references in a local repository",
    .usages = usages,
    .nusages = sizeof(usages) / sizeof(usages[0]),
    .epilog = "By default, local branches and tags are listed.",
};

#endif
