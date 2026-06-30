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
#include <libqgit/db/oid.h>
#include <libqgit/refs.h>
#include <libqgit/types.h>
#include <stdlib.h>
#include <string.h>

int qgit_reference_resolve(qgit_reference **out, const qgit_reference *ref)
{
    assert(out && ref);

    *out = NULL;

    if (ref->type == QGIT_REF_OID) /* direct reference */
    {
        *out = calloc(1, sizeof(qgit_reference));
        if (!*out)
            return -1;
        (*out)->type = QGIT_REF_OID;
        (*out)->owner = ref->owner;
        (*out)->name = strdup(ref->name);
        if (!(*out)->name) {
            qgit_reference_free(*out);
            return -1;
        }
        qgit_oid_copy(&(*out)->target.oid, &ref->target.oid);

        return 0;
    } else /* symbolic reference */
    {
        qgit_reference *cur;
        if (qgit_reference_lookup(&cur, ref->owner, ref->target.symbolic) == -1)
            return -1;
        while (cur->type == QGIT_REF_SYMBOLIC) {
            qgit_reference *next;
            if (qgit_reference_lookup(&next, ref->owner,
                                      cur->target.symbolic) == -1) {
                qgit_reference_free(cur);
                return -1;
            }
            qgit_reference_free(cur);
            cur = next;
        }
        *out = cur;
        return 0;
    }
}