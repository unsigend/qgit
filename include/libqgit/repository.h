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

/* Open a repository at the exact given path. The 'path' argument must point to
   either a qgit repository folder, or an existing work dir. */
QGIT_EXTERN(int) qgit_repository_open(qgit_repository **repo, const char *path);

/* Walk parent directories from start_path to find a repository.
   Writes the found .qgit directory path into repo_path (size bytes). */
QGIT_EXTERN(int)
qgit_repository_discover(char *repo_path, size_t size, const char *start_path);

/* Initialize a new repository at path with the given initial branch name. The
   path must be an absolute path.*/
QGIT_EXTERN(int)
qgit_repository_init(qgit_repository **repo, const char *path,
                     const char *branch);

/* Free a previously allocated repository. */
QGIT_EXTERN(void) qgit_repository_free(qgit_repository *repo);

/* Get the path to the .qgit directory. */
QGIT_EXTERN(const char *) qgit_repository_path(const qgit_repository *repo);

/* Get the path to the working directory. */
QGIT_EXTERN(const char *) qgit_repository_workdir(const qgit_repository *repo);

/* Return 1 if the repository has no commits, 0 otherwise. */
QGIT_EXTERN(int) qgit_repository_is_empty(qgit_repository *repo);

/* Retrieve and resolve the reference pointed at by HEAD. */
QGIT_EXTERN(int)
qgit_repository_head(qgit_reference **head, qgit_repository *repo);

/* Return 1 if HEAD points directly to a commit (detached), 0 otherwise. */
QGIT_EXTERN(int) qgit_repository_head_detached(qgit_repository *repo);

/* Return 1 if HEAD points to a branch that has no commits (unborn), 0
   otherwise. */
QGIT_EXTERN(int) qgit_repository_head_unborn(qgit_repository *repo);

/* Get the object database for this repository. */
QGIT_EXTERN(int) qgit_repository_odb(qgit_odb **out, qgit_repository *repo);

/* Get the index for this repository. */
QGIT_EXTERN(int) qgit_repository_index(qgit_index **out, qgit_repository *repo);

/* Get the config for this repository. */
QGIT_EXTERN(int)
qgit_repository_config(qgit_config **out, qgit_repository *repo);

END_DECLS

#endif
