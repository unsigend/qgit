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

#ifndef CREDENTIALS_H
#define CREDENTIALS_H

/* Wrapper layer for credentials */

#include <libqgit/object/signature.h>

/* Get credentials from the local repository and global config file. Return 0 on
 * success, -1 on error and set errno. */
extern int credentials_get(qgit_signature **out, qgit_repository *repo);

#endif