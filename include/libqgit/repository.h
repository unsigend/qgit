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

#ifndef LIBQGIT_REPOSITORY_H
#define LIBQGIT_REPOSITORY_H

#include <libqgit/common.h>
#include <libqgit/types.h>
#include <stddef.h>

BEGIN_DECLS

/**
 * Open a qgit repository.
 *
 * The `path` argument must point to either a qgit repository
 * folder, or an existing work dir.
 *
 * @param repo pointer to the repo which will be opened
 * @param path the path to the repository
 * @return 0 on success, -1 on error and set errno
 */
QGIT_EXTERN(int) qgit_repository_open(qgit_repository **repo, const char *path);

/**
 * Look for a qgit repository and copy its path in the given buffer.
 *
 * The lookup starts from `start_path` and walks across parent directories
 * if nothing has been found. The lookup ends when the first repository
 * is found.
 *
 * @param repo_path the user allocated buffer which will contain the found path
 * @param size size of the `repo_path` buffer
 * @param start_path the base path where the lookup starts
 * @return 0 on success, -1 on error and set errno
 */
QGIT_EXTERN(int)
qgit_repository_discover(char *repo_path, size_t size, const char *start_path);

/**
 * Initialize a new repository at the given path.
 *
 * The path must be an absolute path. The repository is created with
 * the given initial branch name.
 *
 * @param repo pointer to the repo which will be created
 * @param path the path to the repository
 * @param branch the name of the initial branch
 * @return 0 on success, -1 on error and set errno
 */
QGIT_EXTERN(int)
qgit_repository_init(qgit_repository **repo, const char *path,
                     const char *branch);

/**
 * Free a previously allocated repository.
 *
 * @param repo repository handle to close. If NULL nothing occurs
 */
QGIT_EXTERN(void) qgit_repository_free(qgit_repository *repo);

/**
 * Get the path of this repository.
 *
 * This is the path of the `.qgit` directory.
 *
 * @param repo a repository object
 * @return the path to the repository
 */
QGIT_EXTERN(const char *) qgit_repository_path(const qgit_repository *repo);

/**
 * Get the path of the working directory for this repository.
 *
 * @param repo a repository object
 * @return the path to the working dir, if it exists
 */
QGIT_EXTERN(const char *) qgit_repository_workdir(const qgit_repository *repo);

/**
 * Check if a repository is empty.
 *
 * An empty repository has just been initialized and contains
 * no commits.
 *
 * @param repo repo to test
 * @return 1 if the repository is empty, 0 if it is not, -1 on error and set
 * errno
 */
QGIT_EXTERN(int) qgit_repository_is_empty(qgit_repository *repo);

/**
 * Retrieve and resolve the reference pointed at by HEAD.
 *
 * @param head pointer to the reference which will be retrieved
 * @param repo a repository object
 * @return 0 on success, -1 on error and set errno
 */
QGIT_EXTERN(int)
qgit_repository_head(qgit_reference **head, qgit_repository *repo);

/**
 * Check if a repository's HEAD is detached.
 *
 * A repository's HEAD is detached when it points directly to a commit
 * instead of a branch.
 *
 * @param repo repo to test
 * @return 1 if HEAD is detached, 0 if it is not, -1 on error and set errno
 */
QGIT_EXTERN(int) qgit_repository_head_detached(qgit_repository *repo);

/**
 * Check if the current branch is unborn.
 *
 * An unborn branch is one named from HEAD but which does not have any
 * commit to point to.
 *
 * @param repo repo to test
 * @return 1 if the current branch is unborn, 0 if it is not, -1 on error and
 * set errno
 */
QGIT_EXTERN(int) qgit_repository_head_unborn(qgit_repository *repo);

/**
 * Get the Object Database for this repository.
 *
 * The ODB must be freed once it is no longer being used by
 * the user.
 *
 * @param out pointer to store the loaded ODB
 * @param repo a repository object
 * @return 0 on success, -1 on error and set errno
 */
QGIT_EXTERN(int) qgit_repository_odb(qgit_odb **out, qgit_repository *repo);

/**
 * Get the Index file for this repository.
 *
 * The index must be freed once it is no longer being used by
 * the user.
 *
 * @param out pointer to store the loaded index
 * @param repo a repository object
 * @return 0 on success, -1 on error and set errno
 */
QGIT_EXTERN(int) qgit_repository_index(qgit_index **out, qgit_repository *repo);

/**
 * Get the configuration file for this repository.
 *
 * The configuration file must be freed once it is no longer
 * being used by the user.
 *
 * @param out pointer to store the loaded config file
 * @param repo a repository object
 * @return 0 on success, -1 on error and set errno
 */
QGIT_EXTERN(int)
qgit_repository_config(qgit_config **out, qgit_repository *repo);

END_DECLS

#endif
