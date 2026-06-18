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

#include <stdio.h>

#include "cmd.h"
#include "feature.h"

int cmd_help(int argc, char **argv)
{
  (void)argc;
  (void)argv;

  printf("Usage: %s <command> [options]\n", PROG_NAME);
  printf("Subcommands:\n");
  for (size_t i = 0; i < subcmds_cnt; i++) {
    printf("  %-15s %s\n", subcmds[i].name, subcmds[i].desc);
  }
  fputc('\n', stdout);
  return 0;
}