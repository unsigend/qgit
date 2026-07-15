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
#include <libqgit/repo/config.h>
#include <stdlib.h>

int qgit_config_get_bool(int *out, qgit_config *cfg, const char *name)
{
    assert(out && cfg && name);
    *out = 0;

    char *copy = strdup(name);
    const char *value;

    if (!copy)
        return -1;

    char *sec, *key;

    if (parse_seckey(copy, &sec, &key) < 0) {
        free(copy);
        return -1;
    }

    value = iniparse_get(cfg->inifp, sec, key);
    if (!value) {
        free(copy);
        return -1;
    }

    if (strcasecmp(value, "true") == 0 || strcasecmp(value, "yes") == 0 ||
        strcasecmp(value, "on") == 0 || strcasecmp(value, "1") == 0)
        *out = 1;
    else if (strcasecmp(value, "false") == 0 || strcasecmp(value, "no") == 0 ||
             strcasecmp(value, "off") == 0 || strcasecmp(value, "0") == 0)
        *out = 0;
    else {
        free(copy);
        return -1;
    }

    free(copy);

    return 0;
}
