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

#ifndef CMD_REV_PARSE_H
#define CMD_REV_PARSE_H

#include <argparse.h>

static struct argparse_opt options[] = {
    OPT_HELP(),
    OPT_END(),
};

static const char *usages[] = {
    "qgit rev-parse [options] <args>",
};

static struct argparse_desc desc = {
    .prog = "qgit rev-parse",
    .desc = "Pick out and massage parameters",
    .usages = usages,
    .nusages = sizeof(usages) / sizeof(usages[0]),
    .epilog = "Resolves each revision argument to a full 40-character object "
              "name. Name resolution follows qgit revision syntax.",
};

#endif
