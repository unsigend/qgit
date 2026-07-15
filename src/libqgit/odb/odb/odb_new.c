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

#include "odb.h"

#include <assert.h>
#include <stdlib.h>

static void backend_free(void *data)
{
    if (!data)
        return;
    struct backend_entry *entry = (struct backend_entry *)data;
    if (entry->backend)
        entry->backend->free(entry->backend); /* delegate */
}

int qgit_odb_new(qgit_odb **out)
{
    assert(out);
    qgit_odb *odb;

    odb = calloc(1, sizeof(qgit_odb));
    if (!odb)
        return -1;

    if (vec_init(&odb->backends, sizeof(struct backend_entry), backend_free) <
        0) /* store the pointers to struct backend */
    {
        free(odb);
        return -1;
    }

    *out = odb;

    return 0;
}
