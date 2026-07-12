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

#ifndef CMD_UPDATE_REF_H
#define CMD_UPDATE_REF_H

#include <argparse.h>

#define ALL_ZERO_OID "0000000000000000000000000000000000000000"

struct cmd_update_ref_flags {
    int delete;   /* -d --delete */
    int no_deref; /* --no-deref */
};

static struct cmd_update_ref_flags flags = {
    .delete = 0,
    .no_deref = 0,
};

static struct argparse_opt options[] = {
    OPT_HELP(),
    OPT_BOOL('d', "delete", "Delete the named ref", &flags.delete),
    OPT_BOOL(0, "no-deref",
             "Update or delete the ref itself without following symbolic "
             "pointers",
             &flags.no_deref),
    OPT_END(),
};

static const char *usages[] = {
    "qgit update-ref [--no-deref] <ref> <new-oid> [<old-oid>]",
    "qgit update-ref [--no-deref] -d <ref> [<old-oid>]",
};

static struct argparse_desc desc = {
    .prog = "qgit update-ref",
    .desc = "Update the object name stored in a ref safely",
    .usages = usages,
    .nusages = sizeof(usages) / sizeof(usages[0]),
    .epilog = "With no -d, store <new-oid> in <ref>. Optional <old-oid> is a "
              "compare-and-swap check. Forty zeros mean the null OID.",
};

#endif
