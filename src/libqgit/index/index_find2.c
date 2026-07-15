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

#include "index.h"

#include <assert.h>
#include <libqgit/repo/index.h>
#include <string.h>

int qgit_index_find2(qgit_index *index, const char *path, int stage)
{
    assert(index && path);

    for (unsigned int i = 0; i < vec_size(index->entries); i++) {
        qgit_index_entry *entry = (qgit_index_entry *)vec_at(index->entries, i);
        if (strcmp(entry->path, path) == 0 &&
            qgit_index_entry_stage(entry) == stage)
            return i;
    }

    return -1;
}
