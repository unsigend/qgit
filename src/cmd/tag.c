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
#include <limits.h>
#include <stdio.h>

#include "argparse.h"
#include "die.h"
#include "obj/obj.h"
#include "ref.h"
#include "repo.h"
#include "sha1.h"

static int print_tag(const char *refname, const unsigned char *sha1, void *arg)
{
  (void)arg;
  (void)sha1;
  if (!refname) {
    errno = EINVAL;
    return -1;
  }

  char tagname[PATH_MAX];
  if (sscanf(refname, "refs/tags/%s", tagname) != 1) {
    errno = EINVAL;
    return -1;
  }
  printf("%s\n", tagname);
  return 0;
}

int cmd_tag(int argc, char **argv)
{
  int d = 0;
  int l = 0;
  const char *name = NULL;
  const char *commit = "HEAD";
  char path[PATH_MAX];

  struct argparse ctx;
  struct argparse_opt opts[] = {
      OPT_HELP(),
      OPT_BOOL('l', "list", "list all tags", &l),
      OPT_BOOL('d', "delete", "delete the tag", &d),
      OPT_END(),
  };

  static const char *usages[] = {
      "qgit tag [options] <tagname> [<commit>]",
      "qgit tag [options] -d <tagname>",
  };

  struct argparse_desc desc = {
      .prog = "qgit tag",
      .desc = "Create, list, or delete tags",
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

  if (d) /* delete mode */
  {
    if (argparse_getremargc(&ctx) < 1)
      die("tag name is required");
    name = argparse_getremargv(&ctx)[0];
    if (snprintf(path, sizeof(path), "%s/refs/tags/%s", repo->gitdir, name) >=
        PATH_MAX) {
      errno = ENAMETOOLONG;
      die_errno();
    }
    if (remove(path) == -1)
      die_errno();

  } else if (argparse_getremargc(&ctx) >= 1) /* create mode */
  {
    if (argparse_getremargc(&ctx) == 2)
      commit = argparse_getremargv(&ctx)[1];
    name = argparse_getremargv(&ctx)[0];

    if (snprintf(path, sizeof(path), "refs/tags/%s", name) >= PATH_MAX) {
      errno = ENAMETOOLONG;
      die_errno();
    }

    unsigned char sha1[SHA1_DIGEST_LENGTH];
    if (obj_resolve(repo, commit, sha1) == -1)
      die_errno();

    if (ref_update(repo, path, sha1) == -1)
      die_errno();

  } else /* list mode */
  {
    if (ref_foreach(repo, "refs/tags", print_tag, NULL) == -1)
      die_errno();
  }

  repo_free(repo);
  argparse_fini(&ctx);
  return 0;
}