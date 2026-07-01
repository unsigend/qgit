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

#ifndef CMD_H
#define CMD_H

#include <stddef.h>

typedef int (*cmd_func)(int argc, char **argv);

struct subcmd {
    const char *name;
    const char *desc;
    cmd_func func;
};

extern const struct subcmd subcmds[];
extern const size_t subcmds_cnt;

/* Execute a subcommand with a command name and arguments. Return 0 on success,
   non-return if the subcommand is call die() quit the program. */
extern int exec_cmd(int argc, char **argv);

/* Subcommand functions */
extern int cmd_add(int argc, char **argv);
extern int cmd_branch(int argc, char **argv);
extern int cmd_cat_file(int argc, char **argv);
extern int cmd_check_ignore(int argc, char **argv);
extern int cmd_checkout(int argc, char **argv);
extern int cmd_commit(int argc, char **argv);
extern int cmd_hash_object(int argc, char **argv);
extern int cmd_init(int argc, char **argv);
extern int cmd_log(int argc, char **argv);
extern int cmd_ls_files(int argc, char **argv);
extern int cmd_ls_tree(int argc, char **argv);
extern int cmd_rev_parse(int argc, char **argv);
extern int cmd_rm(int argc, char **argv);
extern int cmd_show_ref(int argc, char **argv);
extern int cmd_status(int argc, char **argv);
extern int cmd_tag(int argc, char **argv);
extern int cmd_help(int argc, char **argv);
extern int cmd_config(int argc, char **argv);
extern int cmd_version(int argc, char **argv);

#endif