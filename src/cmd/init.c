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

// #include "argparse.h"
// #include "cmd.h"
// #include "die.h"
// #include "fs.h"
// #include "repo.h"

// int cmd_init(int argc, char **argv)
// {
//   int q = 0;
//   const char *branch = "main";
//   const char *path = ".";

//   struct argparse ctx;
//   struct argparse_opt opts[] = {
//       OPT_HELP(),
//       OPT_BOOL('q', "quiet", "Only print error and warning messages", &q),
//       OPT_STR('b', "initial-branch", "Use <branch-name> for the initial
//       branch",
//               &branch, OPT_REQUIRED),
//       OPT_END(),
//   };

//   static const char *usages[] = {
//       "qgit init [-q | --quiet] [-b <branch-name> | "
//       "--initial-branch=<branch-name>] "
//       "[<directory>]",
//   };

//   struct argparse_desc desc = {
//       .prog = "qgit init",
//       .desc = "Create an empty qgit repository or reinitialize an existing
//       one", .usages = usages, .nusages = sizeof(usages) / sizeof(usages[0]),
//       .epilog = "The default initial branch is 'main'.",
//   };

//   if (argparse_init(&ctx, opts, &desc) == -1)
//     die("%s", argparse_strerror(&ctx));
//   if (argparse_parse(&ctx, argc, argv) == -1)
//     die("%s", argparse_strerror(&ctx));

//   struct repo *repo = NULL;
//   int reinit = 0;
//   char abspath[PATH_MAX];
//   char qgitdir[PATH_MAX];

//   if (argparse_getremargc(&ctx) > 0)
//     path = argparse_getremargv(&ctx)[0];
//   if (argparse_getremargc(&ctx) > 1)
//     die("too many arguments");

//   if (fabspath(path, abspath) == -1)
//     die_errno();

//   if (snprintf(qgitdir, PATH_MAX, "%s/.qgit", abspath) >= PATH_MAX) {
//     errno = ENAMETOOLONG;
//     die_errno();
//   }

//   if (dir_exists(qgitdir))
//     reinit = 1;

//   repo = repo_create(abspath, branch);
//   if (!repo)
//     die_errno();

//   if (!q) {
//     if (reinit)
//       printf("Reinitialized existing repository in %s\n", qgitdir);
//     else
//       printf("Initialized empty repository in %s\n", qgitdir);
//   }

//   repo_close(repo);
//   argparse_fini(&ctx);
//   return 0;
// }