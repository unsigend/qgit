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

#include <assert.h>
#include <libqgit/repo/refs.h>
#include <libqgit/repo/repository.h>
#include <string.h>

int qgit_branch_is_head(const qgit_reference *ref)
{
    assert(ref);

    qgit_reference *head;

    if (qgit_repository_head(&head, qgit_reference_owner(ref)) < 0)
        return -1;

    if (qgit_reference_type(head) == QGIT_REF_DIRECT) /* detached HEAD */
    {
        qgit_reference_free(head);
        return 0;
    }

    int ret = strcmp(qgit_reference_name(ref), qgit_reference_target(head));
    qgit_reference_free(head);

    return ret == 0;
}
