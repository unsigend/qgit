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

#ifndef CMD_CAT_FILE_H
#define CMD_CAT_FILE_H

#include <argparse.h>

struct cmd_cat_file_flags {
    int pretty;     /* -p pretty-print object contents */
    int print_type; /* -t print object type */
    int print_size; /* -s print object size in bytes */
};

static struct cmd_cat_file_flags flags = {
    .pretty = 0,
    .print_type = 0,
    .print_size = 0,
};

static struct argparse_opt options[] = {
    OPT_HELP(),
    OPT_BOOL('p', NULL, "Pretty-print the object contents", &flags.pretty),
    OPT_BOOL('t', NULL, "Print the object type", &flags.print_type),
    OPT_BOOL('s', NULL, "Print the object size in bytes", &flags.print_size),
    OPT_END(),
};

static const char *usages[] = {
    "qgit cat-file (-p | -t | -s) <object>",
    "qgit cat-file <type> <object>",
};

static struct argparse_desc desc = {
    .prog = "qgit cat-file",
    .desc = "Provide contents or details of repository objects",
    .usages = usages,
    .nusages = sizeof(usages) / sizeof(usages[0]),
    .epilog = "Only one of -p, -t, or -s may be used. Supported types: "
              "blob, commit, tree, tag.",
};

#endif
