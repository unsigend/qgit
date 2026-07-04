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
#include <limits.h>
#include <stdlib.h>

int qgit_config_create_global(qgit_config **out)
{
    assert(out);
    *out = NULL;

    char path[PATH_MAX];
    if (qgit_config_find_global(path, PATH_MAX) == -1)
        return -1;

    struct qgit_config *cfg = calloc(1, sizeof(struct qgit_config));
    if (!cfg)
        return -1;

    cfg->inifp = iniparse_create(path);
    if (!cfg->inifp) {
        free(cfg);
        return -1;
    }
    *out = cfg;

    return 0;
}