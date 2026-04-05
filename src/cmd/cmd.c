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

#include "cmd.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const struct subcmd subcmds[] = {
    {"init", "Initialize a new repository", cmd_init},
    {"add", "Add file contents to the index", cmd_add},
    {"status", "Show the working tree status", cmd_status},
    {"commit", "Record changes to the repository", cmd_commit},
    {"log", "Show commit logs", cmd_log},
    {"cat-file",
     "Provide content or type and size information for repository objects",
     cmd_cat_file},
    {"check-ignore", "Check if a file is ignored", cmd_check_ignore},
    {"checkout", "Checkout a branch or paths to the working tree",
     cmd_checkout},
    {"hash-object",
     "Compute the object ID and optionally creates a blob from a file",
     cmd_hash_object},
    {"ls-files", "List files in the index and the working tree", cmd_ls_files},
    {"ls-tree", "List the contents of a tree object", cmd_ls_tree},
    {"rev-parse", "Parse revision or ref arguments", cmd_rev_parse},
    {"rm", "Remove files from the working tree and from the index", cmd_rm},
    {"show-ref", "List references in a repository", cmd_show_ref},
    {"tag", "Create, list, delete or verify a tag object signed with GPG",
     cmd_tag},
    {"merge", "Join two or more development histories together", cmd_merge},
    {"branch", "List, create, or delete branches", cmd_branch},
};

const size_t subcmdslen = sizeof(subcmds) / sizeof(subcmds[0]);

void showcmds(void)
{
  puts("qgit - A simplified git like version control system\n");
  puts("Usage: qgit <subcommand> [options]\n");
  puts("Subcommands:");
  for (size_t i = 0; i < subcmdslen; i++)
    printf("  %-16s %s\n", subcmds[i].name, subcmds[i].desc);
  putc('\n', stdout);
}

int runcmd(int argc, char **argv)
{
  struct subcmd *subcmd = NULL;

  for (size_t i = 0; i < subcmdslen; i++) {
    if (!strcmp(subcmds[i].name, argv[0])) {
      subcmd = (struct subcmd *)&subcmds[i];
      break;
    }
  }

  if (!subcmd) {
    if (!strcmp(argv[0], "--help") || !strcmp(argv[0], "-h")) {
      showcmds();
      return EXIT_SUCCESS;
    } else
      error("qgit: '%s' is not a qgit command. See 'qgit --help'\n", argv[0]);
  }

  return subcmd->fn(argc - 1, argv + 1);
}