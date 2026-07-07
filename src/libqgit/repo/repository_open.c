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

#include <assert.h>
#include <collection/string.h>
#include <libqgit/db/odb.h>
#include <libqgit/error.h>
#include <libqgit/repo/config.h>
#include <libqgit/repo/index.h>
#include <libqgit/repo/repository.h>
#include <stdlib.h>
#include <string.h>

static int load_config(qgit_repository *repo)
{
    char config[PATH_MAX];
    if (snprintf(config, PATH_MAX, "%s/config", repo->repodir) >= PATH_MAX) {
        errno = ENAMETOOLONG;
        return -1;
    }

    return qgit_config_open_ondisk(&repo->config, config);
}

// static int load_index(qgit_repository *repo)
// {
//     char index[PATH_MAX];
//     if (snprintf(index, PATH_MAX, "%s/index", repo->repodir) >= PATH_MAX) {
//         errno = ENAMETOOLONG;
//         return -1;
//     }

//     return qgit_index_open(&repo->index, index);
// }

static int load_odb(qgit_repository *repo)
{
    char objects_dir[PATH_MAX];
    if (snprintf(objects_dir, PATH_MAX, "%s/objects", repo->repodir) >=
        PATH_MAX) {
        errno = ENAMETOOLONG;
        return -1;
    }

    return qgit_odb_open(&repo->odb, objects_dir);
}

int qgit_repository_open(qgit_repository **out, const char *path)
{
    assert(out && path);

    *out = NULL;

    qgit_repository *r;
    char repodir[PATH_MAX];
    char workdir[PATH_MAX];

    if (strlen(path) >= PATH_MAX) {
        errno = ENAMETOOLONG;
        return -1;
    }

    if (str_endswith(path, "/.qgit") ||
        str_endswith(path, "/.qgit/")) /* path is repository directory */
    {
        strcpy(repodir, path);
        strcpy(workdir, path);
        if (str_endswith(workdir, "/.qgit/"))
            workdir[strlen(workdir) - 1] = '\0';
        if (str_endswith(repodir, "/.qgit/"))
            repodir[strlen(repodir) - 1] = '\0';
        *strrchr(workdir, '/') = '\0';
    } else /* path is work directory */
    {
        if (snprintf(repodir, PATH_MAX, "%s/.qgit", path) >= PATH_MAX) {
            errno = ENAMETOOLONG;
            return -1;
        }

        strcpy(workdir, path);
    }

    if (!dir_exists(repodir)) {
        qgit_seterror(QGITERR_REPONOTFOUND);
        return -1;
    }

    r = calloc(1, sizeof(qgit_repository));
    if (!r)
        return -1;

    r->repodir = strdup(repodir);
    r->workdir = strdup(workdir);

    if (!r->repodir || !r->workdir) {
        qgit_repository_free(r);
        return -1;
    }

    /* TODO: load the index */
    if (load_config(r) || load_odb(r)) {
        qgit_repository_free(r);
        return -1;
    }

    *out = r;

    return 0;
}
