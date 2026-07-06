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

#include "blob.h"

#include <assert.h>
#include <libqgit/db/odb.h>

int blob_parse(qgit_blob *out, qgit_odb_object *odb_obj)
{
    assert(out && odb_obj);
    out->data = (void *)qgit_odb_object_data(odb_obj);
    out->len = qgit_odb_object_size(odb_obj);
    return 0;
}