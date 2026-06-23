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
#include <string.h>
#include <time.h>

#include "argparse.h"
#include "config.h"
#include "die.h"
#include "error.h"
#include "fs.h"
#include "iniparse.h"
#include "obj/object.h"
#include "obj/tag.h"
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

static int ref_name_cmp(const void *a, const void *b)
{
  const struct ref *ra = a, *rb = b;
  return strcmp(ra->name, rb->name);
}

int cmd_tag(int argc, char **argv)
{
  int l, d, f, a;
  l = d = f = a = 0;
  const char *tagname = NULL;
  const char *commit = "HEAD";
  const char *msg = NULL;

  struct argparse ctx;
  struct argparse_opt opts[] = {
      OPT_HELP(),
      OPT_BOOL('l', "list", "list all tags", &l),
      OPT_BOOL('d', "delete", "delete the tag", &d),
      OPT_BOOL('f', "force", "force overwrite existing tag", &f),
      OPT_STR('m', "message", "use the given tag message", &msg, OPT_REQUIRED),
      OPT_BOOL('a', "annotate", "make an annotated tag", &a),
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
    if (tagname[0] == '.' || tagname[0] == '/')
      die("invalid tag name");

    if (snprintf(buf, PATH_MAX, "%s/refs/tags/%s", repo->qgitdir, tagname) >=
        PATH_MAX) {
      errno = ENAMETOOLONG;
      die_errno();
    }

    if (remove(buf) == -1)
      die_errno();

  } else if (l ||
             (!argparse_getremargc(&ctx) && !f && !a && !msg)) /* list mode */
  {
    struct vector_iter iter;
    struct refs refs;
    struct ref *ref = NULL;

    if (refs_init(&refs, repo) == -1)
      die_errno();

    if (vec_iter_init(&iter, &refs.tags) == -1)
      die_errno();

    vec_sort(&refs.tags, ref_name_cmp);
    while (vec_iter_get(&iter)) {
      ref = vec_iter_get(&iter);
      if (printtag(ref->name, ref->sha1) == -1)
        die_errno();
      vec_iter_inc(&iter);
    }

    refs_fini(&refs);

  } else if (!d && !l) /* create mode */
  {
    if (argparse_getremargc(&ctx) < 1)
      die("missing tag name");
    tagname = argparse_getremargv(&ctx)[0];
    if (tagname[0] == '.' || tagname[0] == '/')
      die("invalid tag name");
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

    if (a) /* annotated tag */
    {
      if (!msg)
        die("missing tag message");

      struct iniFILE *lcfg = NULL;
      struct iniFILE *gcfg = NULL;
      const char *name, *email;

      if (!(lcfg = config_repo(repo)))
        die_errno();
      gcfg = config_global();

      name = iniparse_get(lcfg, "user", "name");
      email = iniparse_get(lcfg, "user", "email");

      if (!name && gcfg)
        name = iniparse_get(gcfg, "user", "name");
      if (!email && gcfg)
        email = iniparse_get(gcfg, "user", "email");

      if (!name || !email)
        die("Author identity unknown\n\n Run\n  qgit config --global "
            "--set user.email <email>\n  qgit config --global --set user.name "
            "<name>\n\nOmit --global to set the identity only in this "
            "repository.");

      struct tag tag;
      struct sign tagger;
      struct obj *obj = NULL, *refobj = NULL;

      if (!(refobj = obj_open(repo, sha1)))
        die_errno();
      if (refobj->type != OBJ_COMMIT) {
        obj_close(refobj);
        die("qgit annotated tag must refer to a commit");
      }
      obj_close(refobj);

      if (sign_init_now(&tagger, name, email) == -1)
        die_errno();

      tag.msg = msg;
      tag.type = "commit"; /* only support commit for tag types */
      tag.name = tagname;
      tag.tagger = tagger;

      sha1_copy(sha1, tag.object);

      if (!(obj = tag_create(&tag)))
        die_errno();
      if (obj_write(obj, repo) == -1)
        die_errno();

      if (ref_update(repo, buf, obj->sha1) == -1)
        die_errno();

      obj_close(obj);
      iniparse_close(lcfg);
      iniparse_close(gcfg);

    } else /* lightweight tag */
    {
      if (ref_update(repo, buf, sha1) == -1)
        die_errno();
    }
  }

  repo_close(repo);
  argparse_fini(&ctx);
  return 0;
}