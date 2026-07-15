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

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <fileutils.h>
#include <libqgit/object/blob.h>
#include <libqgit/object/tree.h>
#include <libqgit/repo/repository.h>
#include <limits.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

int qgit_checkout_tree(qgit_repository *repo, const qgit_tree *tree,
                       const char *path)
{
    assert(repo && tree && path);

    if (mkdir(path, QGIT_DIRMODE) < 0)
        return -1;

    for (size_t i = 0; i < qgit_tree_entrycount((qgit_tree *)tree); i++) {

        const qgit_tree_entry *entry =
            qgit_tree_entry_byindex((qgit_tree *)tree, i);
        char fullpath[PATH_MAX];

        if (snprintf(fullpath, PATH_MAX, "%s/%s", path,
                     qgit_tree_entry_name(entry)) >= PATH_MAX) {
            errno = ENAMETOOLONG;
            return -1;
        }

        if (qgit_tree_entry_type(entry) == QGIT_OBJ_BLOB) {
            qgit_blob *blob;
            int fd;
            if (qgit_blob_lookup(&blob, repo, qgit_tree_entry_id(entry)) < 0)
                return -1;
            /* write the blob to the disk */
            if ((fd = open(fullpath, O_WRONLY | O_CREAT | O_TRUNC,
                           QGIT_FILEMODE)) < 0) {
                qgit_blob_free(blob);
                return -1;
            }
            if (write_all(fd, qgit_blob_rawcontent(blob),
                          qgit_blob_rawsize(blob)) < 0) {
                close(fd);
                qgit_blob_free(blob);
                return -1;
            }
            close(fd);
            qgit_blob_free(blob);
        } else if (qgit_tree_entry_type(entry) ==
                   QGIT_OBJ_TREE) /* recursively checkout the tree */
        {
            qgit_tree *subtree;
            if (qgit_tree_lookup(&subtree, repo, qgit_tree_entry_id(entry)) < 0)
                return -1;
            if (qgit_checkout_tree(repo, subtree, fullpath) < 0) {
                qgit_tree_free(subtree);
                return -1;
            }
            qgit_tree_free(subtree);
        }

        else
            return -1;
    }

    return 0;
}