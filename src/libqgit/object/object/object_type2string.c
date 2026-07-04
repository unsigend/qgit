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

#include <libqgit/error.h>
#include <libqgit/object/object.h>

const char *qgit_object_type2string(qgit_obj_type type)
{
    switch (type) {
    case QGIT_OBJ_COMMIT:
        return "commit";
    case QGIT_OBJ_TREE:
        return "tree";
    case QGIT_OBJ_BLOB:
        return "blob";
    case QGIT_OBJ_TAG:
        return "tag";
    default: {
        qgit_seterror(QGITERR_BADOBJTYPE);
        return NULL;
    }
    }
    return NULL;
}
