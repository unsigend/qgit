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

#include <string.h>

const struct subcmd subcmds[] = {
    {"help", "Show help for a command", cmd_help},
    {"version", "Show version information", cmd_version},
    {"config", "Get and set repository or global options", cmd_config},
    {"add", "Add file contents to the index", cmd_add},
    {"cat-file", "Provide contents or details of repository objects",
     cmd_cat_file},
    {"check-ignore", "Debug gitignore / exclude files", cmd_check_ignore},
    {"checkout", "Switch branches or restore working tree files", cmd_checkout},
    {"commit", "Record changes to the repository", cmd_commit},
    {"hash-object",
     "Compute object ID and optionally create an object from a file",
     cmd_hash_object},
    {"init", "Create an empty Git repository or reinitialize an existing one",
     cmd_init},
    {"log", "Show commit logs", cmd_log},
    {"ls-files",
     "Show information about files in the index and the working tree",
     cmd_ls_files},
    {"ls-tree", "List the contents of a tree object", cmd_ls_tree},
    {"rev-parse", "Pick out and massage parameters", cmd_rev_parse},
    {"rm", "Remove files from the working tree and from the index", cmd_rm},
    {"show-ref", "List references in a local repository", cmd_show_ref},
    {"status", "Show the working tree status", cmd_status},
    {"tag", "Create, list, delete or verify a tag object signed with GPG",
     cmd_tag},
};

const size_t nsubcmds = sizeof(subcmds) / sizeof(subcmds[0]);

int exec_cmd(const char *cmd, int argc, char **argv)
{
  if (!cmd || !*cmd)
    return -1;

  for (size_t i = 0; i < nsubcmds; i++) {
    if (strcmp(cmd, subcmds[i].name) == 0) {
      return subcmds[i].func(argc, argv);
    }
  }

  return -1;
}