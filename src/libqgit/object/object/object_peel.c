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

#include "libqgit/object/commit.h"
#include "libqgit/object/tag.h"
#include <assert.h>
#include <libqgit/error.h>

/* Peel object once, tag to target and commit to tree, else return -1. */
static int peel_once(qgit_object **out, const qgit_object *object)
{
    assert(out && object);

    const qgit_obj_type type = qgit_object_type(object);

    if (type == QGIT_OBJ_TAG) {
        qgit_tag *tag = (qgit_tag *)object;
        return qgit_object_lookup(out, qgit_object_owner(object),
                                  qgit_tag_target_oid(tag), QGIT_OBJ_ANY);
    }

    if (type == QGIT_OBJ_COMMIT) {
        qgit_commit *commit = (qgit_commit *)object;
        return qgit_object_lookup(out, qgit_object_owner(object),
                                  qgit_commit_tree_oid(commit), QGIT_OBJ_TREE);
    }

    qgit_seterror(QGITERR_BADPEEL);
    return -1;
}

int qgit_object_peel(qgit_object **peeled, const qgit_object *object,
                     qgit_obj_type target_type)
{
    assert(peeled && object);

    const qgit_obj_type type = qgit_object_type(object);

    if (type == target_type) /* already the target type */
        return qgit_object_dup(peeled, object);

    qgit_object *cur = (qgit_object *)object;
    qgit_object *next;

    while (peel_once(&next, cur) == 0) {
        if (cur != object)
            qgit_object_free(cur);
        cur = next;
        if (qgit_object_type(cur) == target_type ||
            (target_type == QGIT_OBJ_ANY && qgit_object_type(cur) != type)) {
            *peeled = cur;
            return 0;
        }
    }

    if (cur != object)
        qgit_object_free(cur);
    *peeled = NULL;
    return -1;
}