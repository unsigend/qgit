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

#ifndef LIBQGIT_REPO_REVPARSE_H
#define LIBQGIT_REPO_REVPARSE_H

#include <libqgit/common.h>
#include <libqgit/types.h>

QGIT_BEGIN_DECLS

/**
 * Resolve a revision string to a single object.
 *
 * Supported specs follow qgit revision syntax:
 *   - full 40-character hex SHA-1
 *   - HEAD
 *   - full reference path (e.g. refs/heads/main, refs/tags/v1.0)
 *   - branch or tag name under refs/heads or refs/tags
 *   - abbreviated SHA-1
 *   - optional peel suffix: ^{}, ^{commit}, ^{tree}, ^{tag}, ^{blob}
 *
 * When the same bare name exists as both a branch and a tag, both
 * references must point at the same object.
 *
 * The returned object must be released with qgit_object_free.
 *
 * @param out  output pointer to receive the object handle, must not be NULL
 * @param repo repository to search, must not be NULL
 * @param spec revision string to resolve, must not be NULL
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int)
qgit_revparse_single(qgit_object **out, qgit_repository *repo,
                     const char *spec);

QGIT_END_DECLS

#endif
