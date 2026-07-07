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

#ifndef LIBQGIT_H
#define LIBQGIT_H

#include <libqgit/common.h>
#include <libqgit/db/odb.h>
#include <libqgit/db/odb_backend.h>
#include <libqgit/object/blob.h>
#include <libqgit/object/commit.h>
#include <libqgit/object/object.h>
#include <libqgit/object/signature.h>
#include <libqgit/object/tag.h>
#include <libqgit/object/tree.h>
#include <libqgit/oid.h>
#include <libqgit/repo/branch.h>
#include <libqgit/repo/config.h>
#include <libqgit/repo/index.h>
#include <libqgit/repo/refs.h>
#include <libqgit/repo/repository.h>
#include <libqgit/repo/revwalk.h>
#include <libqgit/types.h>

#endif