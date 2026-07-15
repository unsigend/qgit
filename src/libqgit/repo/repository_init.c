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

#include <iniparse.h>
#include <libqgit/repo/repository.h>
#include <unistd.h>

static const char *dirs[] = {"objects", "refs/heads", "refs/tags",
                             "refs/remotes"};

static const char *description =
    "Unnamed repository; edit this file 'description' to name the repository.";

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
            if (mkdirp(buf, QGIT_DIRMODE) == -1) {
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
            unlink(buf);
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
            unlink(buf);
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
            unlink(buf);
            return -1;
        }

        fclose(fp);
        return 0;
    }

    return 0;
}

int qgit_repository_init(qgit_repository **out, const char *path,
                         const char *branch)
{
    assert(out && path);
    *out = NULL;
    branch = branch ? branch : "main";

    int work_created = 0;    /* worktree dir created */
    int repodir_created = 0; /* repodir dir created */
    char repodir[PATH_MAX];

    if (path_exists(path)) {
        if (file_exists(path)) {
            errno = ENOTDIR;
            return -1;
        }
    } else {
        if (mkdirp(path, QGIT_DIRMODE) == -1)
            return -1;
        work_created = 1;
    }

    if (snprintf(repodir, PATH_MAX, "%s/.qgit", path) >= PATH_MAX) {
        errno = ENAMETOOLONG;
        if (work_created)
            rmdirr(path);
        return -1;
    }

    if (!dir_exists(repodir)) {
        if (mkdirp(repodir, QGIT_DIRMODE) == -1) {
            if (work_created)
                rmdirr(path);
            return -1;
        }
        repodir_created = 1;
    }

    if (create_structure(repodir) < 0 || create_config(repodir) < 0 ||
        create_head(repodir, branch) < 0 || create_description(repodir) < 0) {
        if (work_created)
            rmdirr(path);
        if (repodir_created)
            rmdirr(repodir);
        return -1;
    }

    int ret = qgit_repository_open(out, repodir);
    if (ret == -1) {
        if (work_created)
            rmdirr(path);
        if (repodir_created)
            rmdirr(repodir);
        return -1;
    }
    return 0;
}
