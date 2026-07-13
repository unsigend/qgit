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

#ifndef CMD_ADD_H
#define CMD_ADD_H

#include <argparse.h>
#include <collection/string.h>
#include <die.h>
#include <dirent.h>
#include <errno.h>
#include <libqgit/repo/index.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

static struct argparse_opt options[] = {
    OPT_HELP(),
    OPT_END(),
};

static const char *usages[] = {
    "qgit add [<path>...]",
};

static struct argparse_desc desc = {
    .prog = "qgit add",
    .desc = "Add file contents to the index",
    .usages = usages,
    .nusages = sizeof(usages) / sizeof(usages[0]),
    .epilog = "At least one path is required. Directories are added "
              "recursively.",
};

/* Return 1 if path is .git / .qgit or lies under one of those directories. */
static int path_contain_repo_meta(const char *path)
{
    if (!path || !path[0])
        return 0;
    if (strcmp(path, ".git") == 0 || strcmp(path, QGIT_REPO_DIR_NAME) == 0)
        return 1;
    if (str_startswith(path, ".git/") ||
        str_startswith(path, QGIT_REPO_DIR_NAME "/"))
        return 1;
    if (str_endswith(path, "/.git") ||
        str_endswith(path, "/" QGIT_REPO_DIR_NAME))
        return 1;
    if (strstr(path, "/.git/") || strstr(path, "/" QGIT_REPO_DIR_NAME "/"))
        return 1;
    return 0;
}

/* Join dir and name into a repository-relative path. Treats "." / "" as root.
 */
static void path_join(char *buf, size_t bufsz, const char *dir,
                      const char *name)
{
    int n;

    if (!dir[0] || strcmp(dir, ".") == 0)
        n = snprintf(buf, bufsz, "%s", name);
    else
        n = snprintf(buf, bufsz, "%s/%s", dir, name);

    if (n < 0 || (size_t)n >= bufsz) {
        errno = ENAMETOOLONG;
        die_errno();
    }
}

/* Add path (repository-relative): a regular file, or a directory recursively.
 */
static void add_path(qgit_index *index, const char *path)
{
    DIR *dir;
    struct dirent *entry;
    struct stat st;
    char fullpath[PATH_MAX];

    if (path_contain_repo_meta(path))
        die("can't add '%s' to the index", path);

    if (lstat(path, &st) < 0)
        die_errno();

    if (S_ISREG(st.st_mode)) /* add a regular file */
    {
        if (qgit_index_add(index, path, QGIT_IDXENTRY_STAGE_NORMAL) < 0)
            die_errno();
        return;
    }

    if (!S_ISDIR(st.st_mode))
        return;

    if (!(dir = opendir(path)))
        die_errno();

    while ((entry = readdir(dir))) /* add a directory recursively */
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        if (path_contain_repo_meta(entry->d_name))
            continue;

        path_join(fullpath, PATH_MAX, path, entry->d_name);

        if (lstat(fullpath, &st) < 0)
            die_errno();
        if (S_ISDIR(st.st_mode))
            add_path(index, fullpath);
        else if (S_ISREG(st.st_mode)) {
            if (qgit_index_add(index, fullpath, QGIT_IDXENTRY_STAGE_NORMAL) < 0)
                die_errno();
        }
    }
    closedir(dir);
}

#endif
