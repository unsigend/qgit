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
#include <libqgit/config.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

int qgit_config_get_int(int *out, qgit_config *config, const char *name)
{
    assert(out && config && name);

    char *sec, *key;
    char *copy = strdup(name);
    if (!copy)
        return -1;

    if (parse_seckey(copy, &sec, &key) == -1) {
        free(copy);
        return -1;
    }

    const char *val = iniparse_get(config->fp, sec, key);
    if (!val) {
        free(copy);
        return -1;
    }

    char *end;
    errno = 0;
    long v = strtol(val, &end, 10);
    if (errno || end == val || *end != '\0') {
        free(copy);
        return -1;
    }
    if (v > INT_MAX || v < INT_MIN) {
        free(copy);
        errno = ERANGE;
        return -1;
    }

    *out = (int)v;
    free(copy);
    return 0;
}