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

#include <errno.h>
#include <stdio.h>

#include "argparse.h"
#include "die.h"
#include "ref.h"
#include "sha1.h"

static int print_ref(const char *refname, const unsigned char *sha1, void *arg)
{
  (void)arg;
  if (!refname || !sha1) {
    errno = EINVAL;
    return -1;
  }

  unsigned char hex[SHA1_HEX_LENGTH];
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
    die("%s", ctx.errstr);

  if (argparse_parse(&ctx, argc, argv) == -1)
    die("%s", ctx.errstr);

  struct repo *repo = repo_cwd();
  if (!repo)
    die("not inside a qgit repository");

  unsigned char sha1[SHA1_DIGEST_LENGTH];
  int show_branches = (!branches && !tags) || branches;
  int show_tags = (!branches && !tags) || tags;

  if (head) {
    if (ref_read(repo, "HEAD", sha1) == -1)
      die_errno();
    if (print_ref("HEAD", sha1, NULL) == -1)
      die_errno();
  }

  if (show_branches) {
    if (ref_foreach(repo, "refs/heads", print_ref, NULL) == -1)
      die_errno();
  }

  if (show_tags) {
    if (ref_foreach(repo, "refs/tags", print_ref, NULL) == -1)
      die_errno();
  }

  repo_free(repo);
  argparse_fini(&ctx);
  return 0;
}