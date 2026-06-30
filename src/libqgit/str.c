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

#include <libqgit/str.h>
#include <stddef.h>
#include <string.h>

int qgit_str_endwith(const char *str, const char *suffix)
{
    size_t str_len = strlen(str);
    size_t suffix_len = strlen(suffix);
    return str_len >= suffix_len &&
           strcmp(str + str_len - suffix_len, suffix) == 0;
}

int qgit_str_startswith(const char *str, const char *prefix)
{
    size_t str_len = strlen(str);
    size_t prefix_len = strlen(prefix);
    return str_len >= prefix_len && strncmp(str, prefix, prefix_len) == 0;
}