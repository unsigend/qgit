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

#include <assert.h>
#include <string.h>

#define BUFLEN 64

int qgit_repository_head_detached(qgit_repository *repo)
{
    assert(repo);

    char buf[BUFLEN] = {0};

    if (qgit_repository_head_content(repo, buf, BUFLEN) == -1)
        return 0;

    return strncmp(buf, "ref: ", 5) == 0 ? 0 : 1;
}