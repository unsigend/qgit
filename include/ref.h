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

#ifndef REF_H
#define REF_H

struct repo;

extern int ref_resolve(struct repo *repo, const char *refname,
                       unsigned char *sha1);
enum ref_scope {
  REF_SCOPE_ALL,
  REF_SCOPE_BRANCHES,
  REF_SCOPE_TAGS,
};

typedef int (*ref_foreach_cb)(const char *refname, unsigned char *sha1);

extern int ref_foreach(struct repo *repo, enum ref_scope scope,
                       ref_foreach_cb cb);

/* resolve HEAD */
extern int ref_resolve_head(struct repo *repo, unsigned char *sha1);

/* resolve a ref from full name like refs/heads/main */
extern int ref_resolve_path(struct repo *repo, const char *path,
                            unsigned char *sha1);

/* resolve short sha1, minimum 7 characters */
int ref_resolve_short_sha1(struct repo *repo, const char *refname,
                           unsigned char *sha1);

#endif