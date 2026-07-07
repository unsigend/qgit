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

#include <libqgit/object/tree.h>

int qgit_treebuilder_insert(qgit_tree_entry **entry_out,
                            qgit_treebuilder *builder, const char *filename,
                            const qgit_oid *id, unsigned int attributes)
{
    (void)entry_out;
    (void)builder;
    (void)filename;
    (void)id;
    (void)attributes;
    return 0;
}
