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

#ifndef CMD_LS_FILES_H
#define CMD_LS_FILES_H

#include <argparse.h>

struct cmd_ls_files_flags {
    int cached; /* show cached files in the index (default) */
    int stage;  /* show mode, object, and stage number */
};

static struct cmd_ls_files_flags flags = {
    .cached = 0,
    .stage = 0,
};

static struct argparse_opt options[] = {
    OPT_HELP(),
    OPT_BOOL('c', "cached", "Show cached files in the index (default)",
             &flags.cached),
    OPT_BOOL('s', "stage",
             "Show staged contents' mode bits, object name and stage number",
             &flags.stage),
    OPT_END(),
};

static const char *usages[] = {
    "qgit ls-files [-c | --cached] [-s | --stage] [<path>]",
};

static struct argparse_desc desc = {
    .prog = "qgit ls-files",
    .desc = "Show information about files in the index",
    .usages = usages,
    .nusages = sizeof(usages) / sizeof(usages[0]),
    .epilog = "With no options, prints each cached path. Optional <path> is a "
              "repository-relative prefix match.",
};

#endif
