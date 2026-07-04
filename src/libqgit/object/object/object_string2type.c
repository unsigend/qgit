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
#include <libqgit/error.h>
#include <libqgit/object/object.h>
#include <string.h>

qgit_obj_type qgit_object_string2type(const char *str)
{
    assert(str);

    if (strcmp(str, "commit") == 0)
        return QGIT_OBJ_COMMIT;
    if (strcmp(str, "tree") == 0)
        return QGIT_OBJ_TREE;
    if (strcmp(str, "blob") == 0)
        return QGIT_OBJ_BLOB;
    if (strcmp(str, "tag") == 0)
        return QGIT_OBJ_TAG;

    qgit_seterror(QGITERR_BADOBJTYPE);
    return QGIT_OBJ_BAD;
}
