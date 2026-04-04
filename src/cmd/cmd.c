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

#include <cmd.h>
#include <stddef.h>
#include <string.h>

struct subcmd subcmds[] = {
    {"init", cmd_init},
    {"add", cmd_add},
    {"status", cmd_status},
    {"commit", cmd_commit},
    {"log", cmd_log},
    {"cat-file", cmd_cat_file},
    {"check-ignore", cmd_check_ignore},
    {"checkout", cmd_checkout},
    {"hash-object", cmd_hash_object},
    {"ls-files", cmd_ls_files},
    {"ls-tree", cmd_ls_tree},
    {"rev-parse", cmd_rev_parse},
    {"rm", cmd_rm},
    {"show-ref", cmd_show_ref},
    {"tag", cmd_tag},
    {"merge", cmd_merge},
    {"branch", cmd_branch},
};

int runcmd(int argc, char **argv)
{
  if (!argc || !argv)
    return -1;

  struct subcmd *subcmd = NULL;
  subcmd->name = argv[0];

  for (size_t i = 0; i < sizeof(subcmds) / sizeof(subcmds[0]); i++) {
    if (!strcmp(subcmds[i].name, subcmd->name)) {
      subcmd = &subcmds[i];
      break;
    }
  }

  if (!subcmd) {
    return -1;
  }

  return subcmd->fn(argc, argv);
}