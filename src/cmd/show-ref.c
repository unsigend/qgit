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

#include <stdio.h>
#include <string.h>

#include "argparse.h"
#include "collection/vector.h"
#include "die.h"
#include "ref.h"
#include "repo.h"
#include "sha1.h"

static int printref(const char *refname, unsigned char *sha1)
{
  unsigned char hex[SHA1_HEXLEN];
  if (sha1_to_hex(sha1, hex) == -1)
    return -1;
  printf("%s %s\n", hex, refname);
  return 0;
}

int cmd_show_ref(int argc, char **argv)
{
  int head = 0;
  int branches = 0;
  int tags = 0;
  struct argparse ctx;
  struct argparse_opt opts[] = {
      OPT_HELP(),
      OPT_BOOL(0, "head", "Show the HEAD reference", &head),
      OPT_BOOL(0, "branches", "Show all branches", &branches),
      OPT_BOOL(0, "tags", "Show all tags", &tags),
      OPT_END(),
  };

  static const char *usages[] = {
      "qgit show-ref [--head] [--branches] [--tags]",
  };
  struct argparse_desc desc = {
      .prog = "qgit show-ref",
      .desc = "List references in a local repository",
      .usages = usages,
      .nusages = sizeof(usages) / sizeof(usages[0]),
  };

  if (argparse_init(&ctx, opts, &desc) == -1)
    die("%s", argparse_strerror(&ctx));
  if (argparse_parse(&ctx, argc, argv) == -1)
    die("%s", argparse_strerror(&ctx));

  struct repo *repo = NULL;
  struct refs refs;
  struct vector_iter iter;
  struct ref *ref;
  int show_branches = branches || (!branches && !tags);
  int show_tags = tags || (!branches && !tags);

  if (!((repo = repo_findcwd())))
    die_errno();

  if (refs_init(&refs, repo) == -1)
    die_errno();

  if (head) {
    unsigned char sha1[SHA1_DIGLEN];
    if (ref_resolve_head(repo, sha1) == -1)
      die_errno();
    if (printref("HEAD", sha1) == -1)
      die_errno();
  }
  if (show_branches) {
    vec_sort(&refs.branches, ref_cmp);
    if (vec_iter_init(&iter, &refs.branches) == -1)
      die_errno();
    while (vec_iter_get(&iter)) {
      ref = vec_iter_get(&iter);
      if (printref(ref->name, ref->sha1) == -1)
        die_errno();
      vec_iter_inc(&iter);
    }
  }
  if (show_tags) {
    vec_sort(&refs.tags, ref_cmp);
    if (vec_iter_init(&iter, &refs.tags) == -1)
      die_errno();
    while (vec_iter_get(&iter)) {
      ref = vec_iter_get(&iter);
      if (printref(ref->name, ref->sha1) == -1)
        die_errno();
      vec_iter_inc(&iter);
    }
  }

  refs_fini(&refs);
  repo_close(repo);
  argparse_fini(&ctx);
  return 0;
}