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
#include <errno.h>
#include <fs.h>
#include <iniparse.h>
#include <libqgit/error.h>
#include <libqgit/repository.h>
#include <libqgit/types.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *dirs[] = {"objects", "refs/heads", "refs/tags",
                             "refs/remotes"};

static const char *description =
    "Unnamed repository; edit this file 'description' to name the repository.";

static void rollback(const char *path, const char *repodir, int created)
{
    if (created)
        rmdirr(path);
    else if (repodir)
        rmdirr(repodir);
}

static int create_structure(const char *repodir)
{
    char buf[PATH_MAX];
    for (size_t i = 0; i < sizeof(dirs) / sizeof(dirs[0]); i++) {
        if (snprintf(buf, PATH_MAX, "%s/%s", repodir, dirs[i]) >= PATH_MAX) {
            errno = ENAMETOOLONG;
            return -1;
        }

        if (!dir_exists(buf)) {
            if (path_exists(buf)) {
                errno = ENOTDIR;
                return -1;
            }
            if (mkdirp(buf, QGIT_DIR_MODE) == -1) {
                return -1;
            }
        }
    }
    return 0;
}

static int create_config(const char *repodir)
{
    char buf[PATH_MAX];
    struct iniFILE *inifp = NULL;

    if (snprintf(buf, PATH_MAX, "%s/config", repodir) >= PATH_MAX) {
        errno = ENAMETOOLONG;
        return -1;
    }

    if (!file_exists(buf)) {
        inifp = iniparse_create(buf);
        if (!inifp)
            return -1;

        if (iniparse_set(inifp, "core", "repositoryformatversion", "0") == -1 ||
            iniparse_set(inifp, "core", "filemode", "false") == -1 ||
            iniparse_set(inifp, "core", "bare", "false") == -1) {
            iniparse_close(inifp);
            return -1;
        }

        if (iniparse_write(inifp) == -1) {
            iniparse_close(inifp);
            return -1;
        }

        iniparse_close(inifp);
        return 0;
    }

    return 0;
}

static int create_head(const char *repodir, const char *branch)
{
    char buf[PATH_MAX];
    FILE *fp = NULL;

    if (snprintf(buf, PATH_MAX, "%s/HEAD", repodir) >= PATH_MAX) {
        errno = ENAMETOOLONG;
        return -1;
    }
    if (!file_exists(buf)) {
        fp = fopen(buf, "w");
        if (!fp)
            return -1;

        if (fprintf(fp, "ref: refs/heads/%s\n", branch) < 0) {
            fclose(fp);
            return -1;
        }

        fclose(fp);
        return 0;
    }

    return 0;
}

static int create_description(const char *repodir)
{
    char buf[PATH_MAX];
    FILE *fp = NULL;

    if (snprintf(buf, PATH_MAX, "%s/description", repodir) >= PATH_MAX) {
        errno = ENAMETOOLONG;
        return -1;
    }

    if (!file_exists(buf)) {
        fp = fopen(buf, "w");
        if (!fp)
            return -1;

        if (fprintf(fp, "%s", description) < 0) {
            fclose(fp);
            return -1;
        }

        fclose(fp);
        return 0;
    }

    return 0;
}

int qgit_repository_init(qgit_repository **repo, const char *path,
                         const char *branch)
{
    assert(repo && path);
    *repo = NULL;

    struct qgit_repository *r = NULL;
    int created = 0; /* worktree dir created */
    char repodir[PATH_MAX];

    r = malloc(sizeof(struct qgit_repository));
    QGITERR_CHECK_ALLOC(r);
    memset(r, 0, sizeof(struct qgit_repository));

    if (path_exists(path)) {
        if (file_exists(path)) {
            errno = ENOTDIR;
            qgit_repository_free(r);
            return -1;
        }
    } else {
        if (mkdirp(path, QGIT_DIR_MODE) == -1) {
            qgit_repository_free(r);
            return -1;
        }
        created = 1;
    }

    r->workdir = strdup(path);
    if (!r->workdir) {
        rollback(path, NULL, created);
        qgit_repository_free(r);
        return -1;
    }

    if (snprintf(repodir, PATH_MAX, "%s/.qgit", path) >= PATH_MAX) {
        errno = ENAMETOOLONG;
        rollback(path, NULL, created);
        qgit_repository_free(r);
        return -1;
    }

    r->repodir = strdup(repodir);
    if (!r->repodir) {
        rollback(path, NULL, created);
        qgit_repository_free(r);
        return -1;
    }

    if (!dir_exists(r->repodir)) {
        if (mkdirp(r->repodir, QGIT_DIR_MODE) == -1) {
            rollback(path, r->repodir, created);
            qgit_repository_free(r);
            return -1;
        }
    }

    if (create_structure(r->repodir) < 0 || create_config(r->repodir) < 0 ||
        create_head(r->repodir, branch) < 0 ||
        create_description(r->repodir) < 0) {
        rollback(path, repodir, created);
        qgit_repository_free(r);
        return -1;
    }

    /* TODO: create the object database, index, and config */

    *repo = r;
    return 0;
}