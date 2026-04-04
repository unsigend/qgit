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

typedef int (*subcmd_fn)(int argc, char **argv);

struct subcmd {
  const char *name;
  const char *desc;
  subcmd_fn fn;
};

extern const struct subcmd subcmds[];
extern const size_t subcmdslen;

extern int runcmd(int, char **);
extern void showcmds(void);

/* subcommands */
extern int cmd_init(int, char **);
extern int cmd_add(int, char **);
extern int cmd_status(int, char **);
extern int cmd_commit(int, char **);
extern int cmd_log(int, char **);
extern int cmd_cat_file(int, char **);
extern int cmd_check_ignore(int, char **);
extern int cmd_checkout(int, char **);
extern int cmd_hash_object(int, char **);
extern int cmd_ls_files(int, char **);
extern int cmd_ls_tree(int, char **);
extern int cmd_rev_parse(int, char **);
extern int cmd_rm(int, char **);
extern int cmd_show_ref(int, char **);
extern int cmd_tag(int, char **);
extern int cmd_merge(int, char **);
extern int cmd_branch(int, char **);

#endif