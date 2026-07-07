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

#ifndef CMD_HASH_OBJECT_H
#define CMD_HASH_OBJECT_H

#include <argparse.h>

#define DEFAULT_OBJECT_TYPE "blob"

struct cmd_hash_object_flags {
    int write;        /* write object to the object database */
    const char *type; /* object type */
};

static struct cmd_hash_object_flags flags = {
    .type = DEFAULT_OBJECT_TYPE,
    .write = 0,
};

static struct argparse_opt options[] = {
    OPT_HELP(),
    OPT_STR('t', "type", "Specify the type of object", &flags.type,
            OPT_REQUIRED),
    OPT_BOOL('w', NULL, "Write the object to the object database",
             &flags.write),
    OPT_END(),
};

static const char *usages[] = {
    "qgit hash-object [-t <type>] [-w] [--] <file>...",
};

static struct argparse_desc desc = {
    .prog = "qgit hash-object",
    .desc = "Compute object ID and optionally create an object from a file",
    .usages = usages,
    .nusages = sizeof(usages) / sizeof(usages[0]),
    .epilog = "The default object type is 'blob'.",
};

#endif
