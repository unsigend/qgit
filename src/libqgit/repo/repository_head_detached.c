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

#include "repository.h"

#include <stdlib.h>
#include <string.h>

int qgit_repository_head_detached(qgit_repository *repo)
{
    assert(repo);

    char *buf;
    size_t buflen;
    int detached = 0;

    if (repository_read_head(repo, &buf, &buflen))
        return -1;

    if (!buf || !buflen)
        return -1; /* not a valid HEAD file */

    if (strncmp(buf, "ref: ", 5) != 0)
        detached = 1;

    free(buf);

    return detached;
}
