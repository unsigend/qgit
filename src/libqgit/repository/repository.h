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

#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <libqgit/common.h>
#include <libqgit/types.h>
#include <stddef.h>

struct qgit_repository {
    qgit_odb *odb;
    qgit_index *index;
    qgit_config *config;

    char *workdir;
    char *repodir;
};

/* Read the content of HEAD file in a repository to a buffer. */
int qgit_repository_head_content(const qgit_repository *repo, char *buf,
                                 size_t buflen);

#endif