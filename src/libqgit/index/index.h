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

#ifndef INDEX_H
#define INDEX_H

#include <collection/vector.h>
#include <libqgit/repo/repository.h>

struct qgit_index {
    struct vector *entries;
    unsigned int version; /* version number */
    char *path;           /* path to the index file */
    qgit_repository *owner;
};

#define QGIT_INDEX_VERSION 2
#define QGIT_INDEX_HEADER_SIZE 12
#define QGIT_INDEX_MAGIC "DIRC"
#define QGIT_INDEX_ALIGNMENT 8

/**
 * Parse an index file into a qgit_index structure. Assume the index's entries
 * are valid states.
 *
 * @param index The qgit_index structure to parse into
 * @param buf The buffer to parse from
 * @param buflen The length of the buffer
 * @return 0 on success, -1 on failure
 */
QGIT_INTERNAL(int)
qgit_index_parse(qgit_index *index, const void *buf, size_t buflen);

/**
 * Free an index entry.
 *
 * @param data The index entry to free
 */
QGIT_INTERNAL(void) qgit_index_entry_free(void *data);

#endif