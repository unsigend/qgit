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

#include "config.h"

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#define GLOBAL_CONFIG_NAME ".qgitconfig"

int qgit_config_global_path(char *path, size_t size)
{
    assert(path && size);
    const char *home;

    if (!(home = getenv("HOME"))) {
        if (!errno)
            errno = EINVAL;
        return -1;
    }

    if (snprintf(path, size, "%s/%s", home, GLOBAL_CONFIG_NAME) >= (int)size) {
        errno = ENAMETOOLONG;
        return -1;
    }

    return 0;
}