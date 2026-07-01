// /* qgit - A simplified git like version control system
//  * Copyright (C) 2025 - 2026 Qiu Yixiang
//  *
//  * This program is free software: you can redistribute it and/or modify
//  * it under the terms of the GNU General Public License as published by
//  * the Free Software Foundation, either version 3 of the License, or
//  * (at your option) any later version.
//  *
//  * This program is distributed in the hope that it will be useful,
//  * but WITHOUT ANY WARRANTY; without even the implied warranty of
//  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  * GNU General Public License for more details.
//  *
//  * You should have received a copy of the GNU General Public License
//  * along with this program.  If not, see <https://www.gnu.org/licenses/>.
//  */

// #include <errno.h>
// #include <limits.h>
// #include <stddef.h>
// #include <string.h>
// #include <unistd.h>

// #include "argparse.h"
// #include "die.h"
// #include "error.h"
// #include "fs.h"
// #include "obj/object.h"
// #include "ref.h"
// #include "repo.h"

// #define ASCII_GREEN "\033[32m"
// #define ASCII_RESET "\033[0m"

// static int branch_name_cmp(const void *a, const void *b)
// {
//   const struct ref *ra = a, *rb = b;
//   return strcmp(ra->name, rb->name);
// }

// int cmd_branch(int argc, char **argv)
// {
//   int l, d, f;
//   l = d = f = 0;
//   const char *branchname = NULL;
//   const char *start = "HEAD";

//   struct argparse ctx;
//   struct argparse_opt opts[] = {
//       OPT_HELP(),
//       OPT_BOOL('l', "list", "list all branches", &l),
//       OPT_BOOL('d', "delete", "delete the branch", &d),
//       OPT_BOOL('f', "force", "overwrite existing branch", &f),
//       OPT_END(),
//   };

//   static const char *usages[] = {
//       "qgit branch [options] [<branchname> [<start-point>]]",
//       "qgit branch [options] -d <branchname>",
//   };

//   struct argparse_desc desc = {
//       .prog = "qgit branch",
//       .desc = "List, create, or delete branches",
//       .usages = usages,
//       .nusages = sizeof(usages) / sizeof(usages[0]),
//   };

//   if (argparse_init(&ctx, opts, &desc) == -1)
//     die("%s", argparse_strerror(&ctx));
//   if (argparse_parse(&ctx, argc, argv) == -1)
//     die("%s", argparse_strerror(&ctx));

//   struct repo *repo = NULL;

//   if (!((repo = repo_findcwd())))
//     die_errno();

//   if (d) /* delete mode */
//   {
//     if (argparse_getremargc(&ctx) < 1)
//       die("missing branch name");
//     branchname = argparse_getremargv(&ctx)[0];
//     if (!branchname[0] || branchname[0] == '.' || branchname[0] == '/')
//       die("invalid branch name");

//     char path[PATH_MAX];
//     char buf[NAME_MAX], headname[NAME_MAX] = {0};

//     if (snprintf(path, PATH_MAX, "%s/refs/heads/%s", repo->qgitdir,
//                  branchname) >= PATH_MAX) {
//       errno = ENAMETOOLONG;
//       die_errno();
//     }

//     if (ref_read_head(repo, buf, NAME_MAX) == -1) {
//       if (errno != ENOENT)
//         die_errno();
//     } else {
//       if (!strncmp(buf, "ref: ", 5)) {
//         if (sscanf(buf, "ref: refs/heads/%s", headname) == 1) {
//           if (strcmp(branchname, headname) == 0 && !f)
//             die("cannot delete the current branch");
//         }
//       }
//     }

//     /* the current delete mode don't do branch merging check, so no
//     combination
//        with -f and check branch status, a simple delete will be enough */
//     if (remove(path) == -1)
//       die_errno();

//   } else if (l || (!argparse_getremargc(&ctx) && !f)) /* list mode */
//   {
//     struct refs refs;
//     struct vector_iter iter;
//     struct ref *ref = NULL;
//     int istty = isatty(STDOUT_FILENO);
//     int has_head = 0;
//     char buf[PATH_MAX], headname[NAME_MAX] = {0};

//     if (ref_read_head(repo, buf, PATH_MAX) == -1) {
//       if (errno != ENOENT)
//         die_errno();
//     } else {
//       if (!strncmp(buf, "ref: ", 5) &&
//           sscanf(buf, "ref: refs/heads/%s", headname) == 1)
//         has_head = 1; /* symbolic HEAD */
//     }

//     if (refs_init(&refs, repo) == -1)
//       die_errno();

//     vec_sort(&refs.branches, branch_name_cmp);
//     if (vec_iter_init(&iter, &refs.branches) == -1)
//       die_errno();

//     while (vec_iter_get(&iter)) {
//       ref = vec_iter_get(&iter);
//       char bname[NAME_MAX];
//       if (sscanf(ref->name, "refs/heads/%s", bname) != 1)
//         die("invalid branch name");
//       if (has_head && strcmp(bname, headname) == 0) {
//         if (istty) {
//           printf("*  %s%s%s\n", ASCII_GREEN, bname, ASCII_RESET);
//         } else {
//           printf("*  %s\n", bname);
//         }
//       } else {
//         printf("  %s\n", bname);
//       }
//       vec_iter_inc(&iter);
//     }

//     refs_fini(&refs);

//   } else /* create mode */
//   {
//     if (argparse_getremargc(&ctx) < 1)
//       die("missing branch name");
//     branchname = argparse_getremargv(&ctx)[0];

//     if (!branchname[0] || branchname[0] == '.' || branchname[0] == '/')
//       die("invalid branch name");

//     if (argparse_getremargc(&ctx) >= 2)
//       start = argparse_getremargv(&ctx)[1];

//     char buf[PATH_MAX];
//     struct object *obj = NULL;

//     if (!(obj = obj_find(repo, start, OBJ_COMMIT))) {
//       if (errno || qerrno)
//         die_errno();
//       die("not a commit");
//     }

//     if (snprintf(buf, PATH_MAX, "%s/refs/heads/%s", repo->qgitdir,
//                  branchname) >= PATH_MAX) {
//       errno = ENAMETOOLONG;
//       die_errno();
//     }

//     if (file_exists(buf) || dir_exists(buf)) {
//       if (!f) {
//         setqerrno(QE_EXISTBRANCH);
//         die_errno();
//       }
//     }

//     if (snprintf(buf, PATH_MAX, "refs/heads/%s", branchname) >= PATH_MAX) {
//       errno = ENAMETOOLONG;
//       die_errno();
//     }

//     if (ref_update(repo, buf, obj->sha1) == -1)
//       die_errno();

//     obj_close(obj);
//   }

//   repo_close(repo);
//   argparse_fini(&ctx);
//   return 0;
// }

int cmd_branch(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    return 0;
}