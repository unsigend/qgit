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
#include <stdlib.h>
#include <string.h>

#include "cmd.h"

int main(int argc, char *argv[])
{
  if (argc < 2)
    return cmd_help(argc - 2, argv + 2);

  const char *cmd = argv[1];
  if (exec_cmd(cmd, argc - 2, argv + 2) == -1) {
    fprintf(stderr, "qgit: '%s' is not a qgit command. See 'qgit help'.\n",
            cmd);
    return -1;
  }

  return 0;
}