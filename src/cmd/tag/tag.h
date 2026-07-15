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

#ifndef CMD_TAG_H
#define CMD_TAG_H

#include <argparse.h>
#include <die.h>

#define DEFAULT_TARGET "HEAD"

struct cmd_tag_flags {
    int list;            /* -l --list */
    int delete;          /* -d --delete */
    int force;           /* -f --force */
    int annotate;        /* -a --annotate */
    const char *message; /* -m --message */
};

static struct cmd_tag_flags flags = {
    .list = 0,
    .delete = 0,
    .force = 0,
    .annotate = 0,
    .message = NULL,
};

static struct argparse_opt options[] = {
    OPT_HELP(),
    OPT_BOOL('l', "list", "List tags", &flags.list),
    OPT_BOOL('d', "delete", "Delete existing tags with the given names",
             &flags.delete),
    OPT_BOOL('f', "force",
             "Replace an existing tag with the given name instead of failing",
             &flags.force),
    OPT_BOOL('a', "annotate", "Make an unsigned, annotated tag object",
             &flags.annotate),
    OPT_STR('m', "message", "Use the given tag message", &flags.message,
            OPT_REQUIRED),
    OPT_END(),
};

static const char *usages[] = {
    "qgit tag [options] <tagname> [<commit>]",
    "qgit tag [options] -d <tagname>",
};

static struct argparse_desc desc = {
    .prog = "qgit tag",
    .desc = "Create, list, or delete tags",
    .usages = usages,
    .nusages = sizeof(usages) / sizeof(usages[0]),
    .epilog = "Annotated tags require -a and -m. With no arguments, list "
              "tags. <commit> defaults to HEAD.",
};

static void mutex_check(void)
{
    if (flags.delete && flags.list)
        die("Cannot use -d and -l together");
    if (flags.annotate && !flags.message)
        die("annotated tags require message use -m ");
}

#endif
