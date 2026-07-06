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
#include <collection/vector.h>
#include <dirent.h>
#include <errno.h>
#include <libqgit/repo/refs.h>
#include <libqgit/repo/repository.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

static void reference_free(void *data)
{
    char *name = *(char **)data;
    if (name)
        free(name);
}

static int reference_cmp(const void *a, const void *b)
{
    const char *name1 = *(const char **)a;
    const char *name2 = *(const char **)b;
    return strcmp(name1, name2);
}

static int foreach (struct vector *refs, const char *repo_path,
                    const char *prefix)
{
    DIR *dir;
    struct dirent *entry;
    char dirpath[PATH_MAX], path[PATH_MAX];
    struct stat st;

    if (snprintf(dirpath, PATH_MAX, "%s/%s", repo_path, prefix) >= PATH_MAX) {
        errno = ENAMETOOLONG;
        return -1;
    }

    dir = opendir(dirpath);
    if (!dir)
        return -1;

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        if (snprintf(path, PATH_MAX, "%s/%s", dirpath, entry->d_name) >=
            PATH_MAX) {
            errno = ENAMETOOLONG;
            closedir(dir);
            return -1;
        }

        if (stat(path, &st) < 0) {
            closedir(dir);
            return -1;
        }

        if (S_ISDIR(st.st_mode)) /* recursive to subdir */
        {
            char subdir[PATH_MAX];

            if (snprintf(subdir, PATH_MAX, "%s/%s", prefix, entry->d_name) >=
                PATH_MAX) {
                errno = ENAMETOOLONG;
                closedir(dir);
                return -1;
            }

            if (foreach (refs, repo_path, subdir) < 0) {
                closedir(dir);
                return -1;
            }

        } else if (S_ISREG(st.st_mode)) /* regular file */
        {
            char refname[PATH_MAX];

            if (snprintf(refname, PATH_MAX, "%s/%s", prefix, entry->d_name) >=
                PATH_MAX) {
                errno = ENAMETOOLONG;
                closedir(dir);
                return -1;
            }

            char *name = strdup(refname);
            if (!name) {
                closedir(dir);
                return -1;
            }

            if (vec_pushback(refs, &name) < 0) {
                free(name);
                closedir(dir);
                return -1;
            }

        } else /* bad file */
        {
            closedir(dir);
            return -1;
        }
    }

    closedir(dir);
    return 0;
}

int qgit_reference_list(struct vector **out, qgit_repository *repo,
                        unsigned int flags)
{
    assert(out && repo);

    struct vector *refs;

    if (vec_init(&refs, sizeof(char *), reference_free) < 0)
        return -1;

    if (flags & QGIT_REF_LIST_BRANCHES) {
        if (foreach (refs, qgit_repository_path(repo), "refs/heads") < 0) {
            vec_free(refs);
            return -1;
        }
    }

    if (flags & QGIT_REF_LIST_TAGS) {
        if (foreach (refs, qgit_repository_path(repo), "refs/tags") < 0) {
            vec_free(refs);
            return -1;
        }
    }

    if (flags & QGIT_REF_LIST_REMOTES) {
        if (foreach (refs, qgit_repository_path(repo), "refs/remotes") < 0) {
            vec_free(refs);
            return -1;
        }
    }

    if (vec_size(refs))
        vec_sort(refs, reference_cmp);

    *out = refs;

    return 0;
}
