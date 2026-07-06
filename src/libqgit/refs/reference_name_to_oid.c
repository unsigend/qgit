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

#include "reference.h"

#include <assert.h>
#include <libqgit/repo/refs.h>

int qgit_reference_name_to_oid(qgit_oid *out, qgit_repository *repo,
                               const char *name)
{
    assert(out && repo && name);

    qgit_reference *ref;
    qgit_reference *resolved;

    if (qgit_reference_lookup(&ref, repo, name) < 0)
        return -1;
    if (ref->type != QGIT_REF_DIRECT) {
        if (qgit_reference_resolve(&resolved, ref) < 0) {
            qgit_reference_free(ref);
            return -1;
        }
        qgit_reference_free(ref);
        ref = resolved;
    }

    qgit_oid_cpy(out, &ref->target.oid);
    qgit_reference_free(ref);

    return 0;
}
