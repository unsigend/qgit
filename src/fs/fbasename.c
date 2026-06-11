/* miniutils - A minimal GNU coreutils implementation
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

#include <libgen.h>

#if defined(__APPLE__) || defined(__FreeBSD__)
char *fbasename(const char *path, char *buf) { return basename_r(path, buf); }
#else
#include <limits.h>
#include <string.h>
char *fbasename(const char *path, char *buf)
{
  strncpy(buf, path, PATH_MAX - 1);
  buf[PATH_MAX - 1] = '\0';
  return basename(buf);
}
#endif
