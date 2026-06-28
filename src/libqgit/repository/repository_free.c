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

#include "repository.h"

#include <libqgit/config.h>
#include <libqgit/index.h>
#include <libqgit/object/odb.h>
#include <stdlib.h>
#include <string.h>

void qgit_repository_free(qgit_repository *repo)
{
    if (!repo)
        return;

    if (repo->repodir)
        free(repo->repodir);
    if (repo->workdir)
        free(repo->workdir);
    if (repo->odb)
        qgit_odb_free(repo->odb);
    if (repo->index)
        qgit_index_free(repo->index);
    if (repo->config)
        qgit_config_free(repo->config);

    memset(repo, 0, sizeof(struct qgit_repository));
    free(repo);
}