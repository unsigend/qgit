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

#ifndef LIBQGIT_REPO_REPOSITORY_H
#define LIBQGIT_REPO_REPOSITORY_H

#include <libqgit/common.h>
#include <libqgit/oid.h>
#include <libqgit/types.h>
#include <stddef.h>

QGIT_BEGIN_DECLS

/**
 * Open a qgit repository at the given path.
 *
 * path must be either the working directory containing a .qgit folder
 * or the .qgit folder itself.
 *
 * @param out  output pointer to receive the repository handle, must not be NULL
 * @param path path to the repository or its working directory
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int) qgit_repository_open(qgit_repository **out, const char *path);

/**
 * Walk up the directory tree from start_path to find a qgit repository
 * and write the path of the .qgit directory into the provided buffer.
 *
 * @param out        output buffer to receive the discovered .qgit path
 * @param out_size   size of the output buffer in bytes
 * @param start_path directory from which to begin the search
 * @return 0 on success, -1 if no repository is found or on error and sets errno
 */
QGIT_EXTERN(int)
qgit_repository_discover(char *out, size_t out_size, const char *start_path);

/**
 * Initialize a new qgit repository at the given path.
 *
 * If a repository already exists at path the call succeeds without
 * overwriting existing data.
 *
 * @param out        output pointer to receive the repository handle, must not
 *                   be NULL
 * @param path       path to the working directory to initialize
 * @param branch     name for the initial branch, or NULL to use "main"
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int)
qgit_repository_init(qgit_repository **out, const char *path,
                     const char *branch);

/**
 * Free a repository handle.
 *
 * Objects spawned from this repository are not freed and must be released
 * independently before accessing them becomes undefined behavior.
 *
 * @param repo repository to free, no-op if NULL
 */
QGIT_EXTERN(void) qgit_repository_free(qgit_repository *repo);

/**
 * Retrieve and resolve the reference that HEAD points to.
 *
 * @param out  output pointer to receive the reference handle, must not be NULL
 * @param repo repository to query, must not be NULL
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int)
qgit_repository_head(qgit_reference **out, qgit_repository *repo);

/**
 * Test whether HEAD is detached.
 *
 * HEAD is detached when it points directly to a commit rather than to a
 * branch reference.
 *
 * @param repo repository to test, must not be NULL
 * @return 1 if HEAD is detached, 0 if not, -1 on error and sets errno
 */
QGIT_EXTERN(int) qgit_repository_head_detached(qgit_repository *repo);

/**
 * Test whether HEAD refers to an unborn branch.
 *
 * An unborn branch is one that is named in HEAD but has no commits yet,
 * i.e. the repository has just been initialized.
 *
 * @param repo repository to test, must not be NULL
 * @return 1 if the branch is unborn, 0 if not, -1 on error and sets errno
 */
QGIT_EXTERN(int) qgit_repository_head_unborn(qgit_repository *repo);

/**
 * Test whether a repository is empty.
 *
 * A repository is empty when it has been initialized but contains no
 * commits.
 *
 * @param repo repository to test, must not be NULL
 * @return 1 if empty, 0 if not, -1 on error and sets errno
 */
QGIT_EXTERN(int) qgit_repository_is_empty(qgit_repository *repo);

/**
 * Return the path of the .qgit directory for this repository.
 *
 * The returned pointer is owned by the repository and is valid until
 * qgit_repository_free is called.
 *
 * @param repo repository to query, must not be NULL
 * @return path to the .qgit directory
 */
QGIT_EXTERN(const char *) qgit_repository_path(qgit_repository *repo);

/**
 * Return the path of the working directory for this repository.
 *
 * The returned pointer is owned by the repository and is valid until
 * qgit_repository_free is called.
 *
 * @param repo repository to query, must not be NULL
 * @return path to the working directory
 */
QGIT_EXTERN(const char *) qgit_repository_workdir(qgit_repository *repo);

/**
 * Return the local config associated with this repository.
 *
 * The returned pointer is owned by the repository and is valid until
 * qgit_repository_free is called. Do not free it.
 *
 * @param repo repository to query, must not be NULL
 * @return config handle, or NULL on error and sets errno
 */
QGIT_EXTERN(qgit_config *) qgit_repository_config(qgit_repository *repo);

/**
 * Return the ODB associated with this repository.
 *
 * The returned pointer is owned by the repository and is valid until
 * qgit_repository_free is called. Do not free it.
 *
 * @param repo repository to query, must not be NULL
 * @return ODB handle, or NULL on error and sets errno
 */
QGIT_EXTERN(qgit_odb *) qgit_repository_odb(qgit_repository *repo);

/**
 * Return the index associated with this repository.
 *
 * The returned pointer is owned by the repository and is valid until
 * qgit_repository_free is called. Do not free it.
 *
 * @param repo repository to query, must not be NULL
 * @return index handle, or NULL on error and sets errno
 */
QGIT_EXTERN(qgit_index *) qgit_repository_index(qgit_repository *repo);

QGIT_END_DECLS

#endif
