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

#ifndef CMD_LOG_H
#define CMD_LOG_H

#include <argparse.h>
#include <die.h>
#include <libqgit/object/commit.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define DEFAULT_TARGET "HEAD"
#define UNLIMIT -1
#define ASCII_COLOR_RESET "\033[0m"
#define ASCII_COLOR_YELLOW "\033[33m"
#define INDENT_WIDTH 4

struct cmd_log_flags {
    int oneline;      /* --oneline */
    int first_parent; /* --first-parent */
    int max_count;    /* -n <num>, default is unlimited */
};

static struct cmd_log_flags flags = {
    .oneline = 0,
    .first_parent = 0,
    .max_count = UNLIMIT,
};

static struct argparse_opt options[] = {
    OPT_HELP(),
    OPT_BOOL(0, "oneline",
             "Show each commit on a single line with an abbreviated hash "
             "and the first line of the commit message",
             &flags.oneline),
    OPT_BOOL(0, "first-parent",
             "Follow only the first parent of a merge commit",
             &flags.first_parent),
    OPT_INT('n', NULL, "Limit the number of commits shown", &flags.max_count,
            OPT_REQUIRED),
    OPT_END(),
};

static const char *usages[] = {
    "qgit log [--oneline] [--first-parent] [-n <num>] [<commit>]",
};

static struct argparse_desc desc = {
    .prog = "qgit log",
    .desc = "Show commit logs",
    .usages = usages,
    .nusages = sizeof(usages) / sizeof(usages[0]),
    .epilog = "List commits reachable from <commit>, or from HEAD when "
              "omitted. <commit> defaults to HEAD.",
};

static void print_commit_default(qgit_commit *commit)
{
    int istty = isatty(fileno(stdout));
    char hex[QGIT_OID_HEXSZ + 1];
    const qgit_signature *author = qgit_commit_author(commit);
    struct tm tm;
    time_t adjusted =
        author->when.time + author->when.offset * 60; /* seconds since UTC */
    char datebuf[64];
    const char *msg = qgit_commit_message(commit);

    qgit_oid_fmt(hex, qgit_object_id((qgit_object *)commit));
    hex[QGIT_OID_HEXSZ] = '\0';

    printf("%scommit %s%s\n", istty ? ASCII_COLOR_YELLOW : "", hex,
           istty ? ASCII_COLOR_RESET : ""); /* commit */

    if (qgit_commit_parentcount(commit) > 1) /* merge commit */
    {
        printf("Merge: ");
        for (unsigned int i = 0; i < qgit_commit_parentcount(commit); i++) {
            qgit_oid_fmt(hex, qgit_commit_parent_oid(commit, i));
            hex[QGIT_OID_HEXSZ] = '\0';
            printf("%.7s", hex);
            if (i < qgit_commit_parentcount(commit) - 1)
                putc(' ', stdout);
        }
        printf("\n");
    }

    printf("Author: %s <%s>\n", author->name, author->email); /* author */
    if (!gmtime_r(&adjusted, &tm))
        die_errno();

    if (!strftime(datebuf, sizeof(datebuf), "%a %b %e %H:%M:%S %Y", &tm))
        die_errno();

    printf("Date:   %s %c%.2d%.2d\n", datebuf,
           author->when.offset >= 0 ? '+' : '-', abs(author->when.offset) / 60,
           abs(author->when.offset) % 60); /* date */

    fputc('\n', stdout);

    if (msg) {
        printf("%*s", INDENT_WIDTH, " ");
        while (*msg) {
            if (*msg == '\n') {
                printf("\n");
                if (*(msg + 1)) /* not the last line */
                    printf("%*s", INDENT_WIDTH, " ");
            } else
                putc(*msg, stdout);
            msg++;
        }
    }
    if (msg)
        putc('\n', stdout);
}

static void print_commit_oneline(qgit_commit *commit)
{
    int istty = isatty(fileno(stdout));
    char hex[QGIT_OID_HEXSZ + 1];
    const char *msg = qgit_commit_message(commit);

    qgit_oid_fmt(hex, qgit_object_id((qgit_object *)commit));
    hex[QGIT_OID_HEXSZ] = '\0';

    printf("%s%.7s%s ", istty ? ASCII_COLOR_YELLOW : "", hex,
           istty ? ASCII_COLOR_RESET : "");

    if (msg) {
        while (*msg && *msg != '\n') { /* first line only */
            if (putc(*msg, stdout) == EOF)
                die_errno();
            msg++;
        }
    }
    if (putc('\n', stdout) == EOF)
        die_errno();
}

static void print_commit(qgit_commit *commit)
{
    if (flags.oneline)
        print_commit_oneline(commit);
    else
        print_commit_default(commit);
}

#endif
