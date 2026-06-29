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

#ifndef LIBQGIT_OBJECT_OBJECT_H
#define LIBQGIT_OBJECT_OBJECT_H

#include <libqgit/common.h>
#include <libqgit/types.h>

BEGIN_DECLS

/* Convert a qgit_obj_type to a string object type representation */
QGIT_EXTERN(const char *) qgit_object_type2string(qgit_obj_type type);

/* Convert a string object type representation to a qgit_obj_type */
QGIT_EXTERN(qgit_obj_type) qgit_object_string2type(const char *str);

END_DECLS

#endif