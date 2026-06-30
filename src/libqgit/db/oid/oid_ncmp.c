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

#include <libqgit/db/oid.h>
#include <string.h>

int qgit_oid_ncmp(const qgit_oid *a, const qgit_oid *b, size_t len)
{
    int ret = memcmp(a->id, b->id, len / 2);
    if (ret != 0)
        return ret;
    if (len & 1) {
        unsigned char mask = 0xf0;
        return (int)(a->id[len / 2] & mask) - (int)(b->id[len / 2] & mask);
    }
    return 0;
}