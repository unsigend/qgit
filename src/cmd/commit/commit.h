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

#ifndef CMD_COMMIT_H
#define CMD_COMMIT_H

#include <argparse.h>
#include <libqgit/object/commit.h>
#include <libqgit/repo/branch.h>
#include <libqgit/repo/refs.h>
#include <stdio.h>
#include <string.h>

struct cmd_commit_flags {
    const char *message; /* -m --message */
};

static struct cmd_commit_flags flags = {
    .message = NULL,
};

static struct argparse_opt options[] = {
    OPT_HELP(),
    OPT_STR('m', "message", "Use the given commit message", &flags.message,
            OPT_REQUIRED),
    OPT_END(),
};

static const char *usages[] = {
    "qgit commit -m <message>",
};

static struct argparse_desc desc = {
    .prog = "qgit commit",
    .desc = "Record changes to the repository",
    .usages = usages,
    .nusages = sizeof(usages) / sizeof(usages[0]),
    .epilog = "Create a commit from the current index and advance HEAD. "
              "A message is required via -m.",
};

/* Print the first summary line of `git commit` output. tip must be the
 * resolved HEAD reference: branch tip or detached HEAD. */
static void print_commit_result(qgit_commit *commit, qgit_reference *tip)
{
    char hex[QGIT_OID_HEXSZ + 1];
    const char *branch = qgit_branch_name(tip);
    const char *msg = qgit_commit_message(commit);
    const char *nl = msg ? strchr(msg, '\n') : NULL;
    const char *label = branch ? branch : "detached HEAD";

    qgit_oid_fmt(hex, qgit_commit_id(commit));
    hex[QGIT_OID_HEXSZ] = '\0';

    if (qgit_commit_parentcount(commit) == 0)
        printf("[%s (root-commit) %.7s] ", label, hex);
    else
        printf("[%s %.7s] ", label, hex);

    if (nl)
        printf("%.*s", (int)(nl - msg), msg);
    else if (msg)
        printf("%s", msg);

    putchar('\n');
}

#endif
