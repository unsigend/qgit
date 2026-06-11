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
#include <stdlib.h>
#include <sys/stat.h>

#include "argparse.h"
#include "die.h"
#include "obj/obj.h"

static const char *mode_to_str(const char *mode, unsigned long *m)
{
  if (!mode) {
    errno = EINVAL;
    return NULL;
  }
  char *endptr;
  errno = 0;
  *m = strtoul(mode, &endptr, 8);
  if (endptr == mode || *endptr != '\0') {
    errno = EINVAL;
    return NULL;
  }
  if (errno == ERANGE)
    return NULL;
  if ((*m & S_IFMT) == S_IFDIR)
    return "tree";
  else if ((*m & S_IFMT) == S_IFREG || (*m & S_IFMT) == S_IFLNK)
    return "blob";
  /* 160000: commit which for print only, not supported in qgit for submodule
     refer external repository */
  else if (*m == 0160000)
    return "commit";
  else {
    errno = EINVAL;
    return NULL;
  }
}

static int print_tree(struct tree *tree)
{
  if (!tree) {
    errno = EINVAL;
    return -1;
  }

  for (size_t i = 0; i < vec_size(&tree->entries); i++) {
    struct tree_entry *entry = (struct tree_entry *)vec_at(&tree->entries, i);
    unsigned long m = 0;
    const char *t = mode_to_str(entry->mode, &m);
    if (!t)
      return -1;
    unsigned char hex[SHA1_HEX_LENGTH];
    if (sha1_to_hex(entry->sha1, hex) == -1)
      return -1;
    if (printf("%06.6lo %s %s\t%s\n", m, t, (char *)hex, entry->path) < 0)
      return -1;
  }
  return 0;
}

int cmd_ls_tree(int argc, char **argv)
{
  int r = 0;
  const char *name = "HEAD";
  struct argparse ctx;
  struct argparse_opt opts[] = {
      OPT_HELP(),
      OPT_BOOL('r', NULL, "Recurse into sub-tree", &r),
      OPT_END(),
  };

  struct argparse_desc desc = {
      .prog = "qgit ls-tree",
      .desc = "List the contents of a tree object",
      .usage = "qgit ls-tree [options] <path>",
  };

  if (argparse_init(&ctx, opts, &desc) == -1)
    die("%s", ctx.errstr);

  if (argparse_parse(&ctx, argc, argv) == -1)
    die("%s", ctx.errstr);

  if (argparse_getremargc(&ctx) > 0)
    name = argparse_getremargv(&ctx)[0];

  struct repo *repo = repo_cwd();
  if (!repo)
    die("not inside a qgit repository");

  unsigned char sha1[SHA1_DIGEST_LENGTH];
  if (obj_resolve(repo, name, sha1) == -1)
    die_errno();

  struct obj *obj = obj_open_sha1(repo, sha1);
  if (!obj)
    die_errno();

  if (obj_parse(obj) == -1)
    die_errno();

  if (obj->type == OBJ_TREE) {
    struct tree *tree = &obj->tree;
    if (print_tree(tree) == -1) {
      if (errno == EINVAL)
        die("broken tree object");
      else
        die_errno();
    }
  } else if (obj->type == OBJ_COMMIT) {
    struct obj *tree_obj = obj_open_sha1(repo, obj->commit.tree);
    if (!tree_obj)
      die_errno();

    if (obj_parse(tree_obj) == -1)
      die_errno();

    struct tree *tree = &tree_obj->tree;
    if (print_tree(tree) == -1) {
      if (errno == EINVAL)
        die("broken tree object");
      else
        die_errno();
    }
    obj_close(tree_obj);
  } else
    die("not a tree object");

  obj_close(obj);
  repo_free(repo);
  argparse_fini(&ctx);
  return 0;
}