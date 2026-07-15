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
#include <fileutils.h>
#include <libqgit/repo/index.h>
#include <limits.h>
#include <stddef.h>
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

/* Normalize the path to a repository-relative path. Reject and die the program
 * when the path is invalid or outside the repository. Assume buf is at least
 * PATH_MAX bytes long. workdir must be an absolute path without a trailing
 * slash (except for "/"). */
static void normalize_path(const char *path, const char *workdir, char *buf)
{
    char fullpath[PATH_MAX];
    size_t len = strlen(workdir);
    size_t fullpathlen;

    if (fabspath(path, fullpath) < 0)
        die_errno();

    /* reject .git / .qgit as a path component */
    if (str_endswith(fullpath, "/.git") ||
        str_endswith(fullpath, "/" QGIT_REPO_DIR_NAME) ||
        strstr(fullpath, "/.git/") ||
        strstr(fullpath, "/" QGIT_REPO_DIR_NAME "/"))
        die("can't add '%s' to the index", path);

    if (!str_startswith(fullpath, workdir))
        die("'%s' is outside repository", path);

    fullpathlen = strlen(fullpath);
    if (fullpathlen == len) {
        /* path resolves to the workdir itself */
        if (snprintf(buf, PATH_MAX, ".") >= PATH_MAX) {
            errno = ENAMETOOLONG;
            die_errno();
        }
        return;
    }

    if (fullpath[len] != '/')
        die("'%s' is outside repository", path);

    if (snprintf(buf, PATH_MAX, "%s", fullpath + len + 1) >= PATH_MAX) {
        errno = ENAMETOOLONG;
        die_errno();
    }
}

static void add_path(qgit_index *index, const char *path, const char *workdir)
{
    struct stat st;
    char normpath[PATH_MAX];

    normalize_path(path, workdir, normpath);

    if (lstat(path, &st) < 0)
        die_errno();

    if (S_ISLNK(st.st_mode)) /* skip symbolic links */
        return;

    if (S_ISREG(st.st_mode)) /* regular file */
    {
        if (qgit_index_add(index, normpath, QGIT_IDXENTRY_STAGE_NORMAL) < 0)
            die_errno();
        return;
    }

    if (S_ISDIR(st.st_mode)) /* directory */
    {
        DIR *dir;
        struct dirent *entry;
        char entry_path[PATH_MAX];
        int n;

        if ((dir = opendir(path)) == NULL)
            die_errno();

        while ((entry = readdir(dir)) != NULL) {
            if (strcmp(entry->d_name, ".") == 0 ||
                strcmp(entry->d_name, "..") == 0 ||
                strcmp(entry->d_name, ".git") == 0 ||
                strcmp(entry->d_name, QGIT_REPO_DIR_NAME) == 0)
                continue;

            if (strcmp(path, ".") == 0)
                n = snprintf(entry_path, PATH_MAX, "%s", entry->d_name);
            else
                n = snprintf(entry_path, PATH_MAX, "%s/%s", path,
                             entry->d_name);

            if (n >= PATH_MAX) {
                errno = ENAMETOOLONG;
                die_errno();
            }

            add_path(index, entry_path, workdir);
        }

        closedir(dir);
    }
}

#endif
