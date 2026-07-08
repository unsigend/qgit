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

#ifndef CMD_LS_TREE_H
#define CMD_LS_TREE_H

#include <argparse.h>
#include <die.h>
#include <errno.h>
#include <libqgit/object/tree.h>
#include <limits.h>
#include <stdio.h>

struct cmd_ls_tree_flags {
    int recursive;  /* -r recurse into sub-trees */
    int show_trees; /* -t show tree entries when recursing */
};

static struct cmd_ls_tree_flags flags = {
    .recursive = 0,
    .show_trees = 0,
};

static struct argparse_opt options[] = {
    OPT_HELP(),
    OPT_BOOL('r', NULL, "Recurse into sub-trees", &flags.recursive),
    OPT_BOOL('t', NULL, "Show tree entries even when going to recurse them",
             &flags.show_trees),
    OPT_END(),
};

static const char *usages[] = {
    "qgit ls-tree [-r] [-t] <tree-ish>",
};

static struct argparse_desc desc = {
    .prog = "qgit ls-tree",
    .desc = "List the contents of a tree object",
    .usages = usages,
    .nusages = sizeof(usages) / sizeof(usages[0]),
    .epilog =
        "When <tree-ish> names a commit, qgit peel the tree in that commit."};

#define SKIP_SUBTREE -1 /* skip descending into a subtree */
#define STOP 1          /* stop the traversal on error */
#define CONTINUE 0      /* continue the traversal */

static int pretty_print_tree_entry(const char *root, qgit_tree_entry *entry,
                                   void *payload)
{
    struct cmd_ls_tree_flags *flag = (struct cmd_ls_tree_flags *)payload;
    qgit_obj_type type = qgit_tree_entry_type(entry);
    char hex[QGIT_OID_HEXSZ + 1];
    char fullpath[PATH_MAX];

    qgit_oid_fmt(hex, qgit_tree_entry_id(entry));
    hex[QGIT_OID_HEXSZ] = '\0';

    if (snprintf(fullpath, PATH_MAX, "%s%s%s", root, *root ? "/" : "",
                 qgit_tree_entry_name(entry)) >= PATH_MAX) {
        errno = ENAMETOOLONG;
        return STOP;
    }

    if (type == QGIT_OBJ_TREE) /* tree entry */
    {
        if (flag->recursive) {
            if (flag->show_trees) {
                if (fprintf(stdout, "%.6o %s %s\t%s\n",
                            qgit_tree_entry_attributes(entry), "tree", hex,
                            fullpath) < 0)
                    return STOP;
                return CONTINUE;
            }

        } else {
            if (fprintf(stdout, "%.6o %s %s\t%s\n",
                        qgit_tree_entry_attributes(entry), "tree", hex,
                        fullpath) < 0)
                return STOP;
            return SKIP_SUBTREE;
        }

    } else /* blob entry */
    {
        if (fprintf(stdout, "%.6o %s %s\t%s\n",
                    qgit_tree_entry_attributes(entry), "blob", hex,
                    fullpath) < 0)
            return STOP;
    }

    return CONTINUE;
}
#endif
