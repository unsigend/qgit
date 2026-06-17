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

/*
  qgit config behaviour:
    --list: if --global or --local is specified, list the global or local config
            file, if auto, list global first then local config.

    --get:  if --global or --local is specified, get the value of the key from
            the global or local config file, if auto, get the value from local
            config first, then from global config if not found.

    --set:  if --global or --local is specified, set the value of the key in the
            global or local config file, if auto, set the value in local config.

    --unset: if --global or --local is specified, unset the value of the key in
            the global or local config file, if auto, unset the value in local
            config.
*/
int cmd_config(int argc, char **argv)
{
  (void)argc;
  (void)argv;
  return 0;
}