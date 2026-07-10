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

#include "revwalk.h"

#include <assert.h>
#include <libqgit/repo/refs.h>

int qgit_revwalk_push_head(qgit_revwalk *walk)
{
    assert(walk);

    qgit_oid oid;
    if (qgit_reference_name_to_oid(&oid, walk->repo, "HEAD") < 0)
        return -1;

    return qgit_revwalk_push(walk, &oid);
}
