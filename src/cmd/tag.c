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

#include "argparse.h"
#include "die.h"
#include "error.h"
#include "fs.h"
#include "ref.h"
#include "repo.h"
#include "sha1.h"

static int printtag(const char *refname, unsigned char *sha1)
{
  (void)sha1;
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
  int l, d, f;
  l = d = f = 0;
  const char *tagname = NULL;
  const char *commit = "HEAD";

  struct argparse ctx;
  struct argparse_opt opts[] = {
      OPT_HELP(),
      OPT_BOOL('l', "list", "list all tags", &l),
      OPT_BOOL('d', "delete", "delete the tag", &d),
      OPT_BOOL('f', "force", "force overwrite existing tag", &f),
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
    die("%s", argparse_strerror(&ctx));
  if (argparse_parse(&ctx, argc, argv) == -1)
    die("%s", argparse_strerror(&ctx));

  struct repo *repo = NULL;
  char buf[PATH_MAX];

  if (!((repo = repo_findcwd())))
    die_errno();

  if (d) /* delete mode */
  {
    if (argparse_getremargc(&ctx) < 1)
      die("missing tag name");
    tagname = argparse_getremargv(&ctx)[0];

    if (snprintf(buf, PATH_MAX, "%s/refs/tags/%s", repo->qgitdir, tagname) >=
        PATH_MAX) {
      errno = ENAMETOOLONG;
      die_errno();
    }

    if (remove(buf) == -1)
      die_errno();

  } else if (l || (!d && !l && !argparse_getremargc(&ctx))) /* list mode */
  {
    if (ref_foreach(repo, REF_SCOPE_TAGS, printtag) == -1)
      die_errno();
  } else if (!d && !l) /* create mode */
  {
    if (argparse_getremargc(&ctx) < 1)
      die("missing tag name");
    tagname = argparse_getremargv(&ctx)[0];
    if (argparse_getremargc(&ctx) >= 2)
      commit = argparse_getremargv(&ctx)[1];

    unsigned char sha1[SHA1_DIGLEN];

    if (snprintf(buf, PATH_MAX, "%s/refs/tags/%s", repo->qgitdir, tagname) >=
        PATH_MAX) {
      errno = ENAMETOOLONG;
      die_errno();
    }
    if (file_exists(buf) && !f) {
      setqerrno(QE_EXISTSTAG);
      die_errno();
    }

    if (ref_resolve(repo, commit, sha1) == -1)
      die_errno();
    if (snprintf(buf, PATH_MAX, "refs/tags/%s", tagname) >= PATH_MAX) {
      errno = ENAMETOOLONG;
      die_errno();
    }
    if (ref_update(repo, buf, sha1) == -1)
      die_errno();
  }

  repo_close(repo);
  argparse_fini(&ctx);
  return 0;
}