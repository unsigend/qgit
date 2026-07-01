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

#include "reference.h"

#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <fs.h>
#include <libqgit/repository.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

static int foreach (const char *base, const char *dirname,
                    int (*callback)(const char *name, void *payload),
                    void *payload)
{
    char path[PATH_MAX], refname[PATH_MAX];
    DIR *dir;
    struct dirent *entry;

    if (snprintf(path, PATH_MAX, "%s/%s", base, dirname) >= PATH_MAX) {
        errno = ENAMETOOLONG;
        return -1;
    }

    dir = opendir(path);
    if (dir == NULL)
        return -1;

    while ((entry = readdir(dir))) {
        if (strcmp(entry->d_name, "..") == 0 || strcmp(entry->d_name, ".") == 0)
            continue;
        if (snprintf(path, PATH_MAX, "%s/%s/%s", base, dirname,
                     entry->d_name) >= PATH_MAX) {
            errno = ENAMETOOLONG;
            closedir(dir);
            return -1;
        }
        if (snprintf(refname, PATH_MAX, "%s/%s", dirname, entry->d_name) >=
            PATH_MAX) {
            errno = ENAMETOOLONG;
            closedir(dir);
            return -1;
        }

        if (dir_exists(path)) /* subdirectory */
        {
            if (foreach (base, refname, callback, payload) == -1) {
                closedir(dir);
                return -1;
            }
        } else /* reference file */
        {
            if (callback(refname, payload)) {
                closedir(dir);
                return -1;
            }
        }
    }

    closedir(dir);
    return 0;
}

int qgit_reference_foreach(qgit_repository *repo,
                           int (*callback)(const char *name, void *payload),
                           void *payload)
{
    assert(repo && callback);

    if (foreach (qgit_repository_path(repo), "refs", callback, payload) == -1)
        return -1;

    return 0;
}