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

#include "tag.h"

#include <errno.h>
#include <libqgit/repo/repository.h>
#include <limits.h>
#include <stdio.h>
#include <unistd.h>

int qgit_tag_delete(qgit_repository *repo, const char *tag_name)
{
    assert(repo && tag_name);

    if (qgit_tag_validate_name(tag_name) < 0)
        return -1;

    char refname[PATH_MAX];
    if (snprintf(refname, PATH_MAX, "%s/refs/tags/%s",
                 qgit_repository_path(repo), tag_name) >= PATH_MAX) {
        errno = ENAMETOOLONG;
        return -1;
    }

    return unlink(refname);
}
