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

// #include "argparse.h"
// #include "die.h"
// #include "index.h"
// #include "repo.h"

// int cmd_commit(int argc, char **argv)
// {
//   const char *msg = NULL;

//   struct argparse ctx;
//   struct argparse_opt opts[] = {
//       OPT_HELP(),
//       OPT_STR('m', "message", "use the given commit message", &msg,
//               OPT_REQUIRED),
//       OPT_END(),
//   };

//   static const char *usages[] = {
//       "qgit commit -m <message>",
//   };

//   struct argparse_desc desc = {
//       .prog = "qgit commit",
//       .desc = "Record changes to the repository",
//       .usages = usages,
//       .nusages = sizeof(usages) / sizeof(usages[0]),
//   };

//   if (argparse_init(&ctx, opts, &desc) == -1)
//     die("%s", argparse_strerror(&ctx));
//   if (argparse_parse(&ctx, argc, argv) == -1)
//     die("%s", argparse_strerror(&ctx));

//   if (argparse_getremargc(&ctx) == 0)
//     die("requires a commit message");

//   struct repo *repo = NULL;
//   struct index *index = NULL;

//   if (!((repo = repo_findcwd())))
//     die_errno();
//   if (!((index = index_open(repo))))
//     die_errno();

//   argparse_fini(&ctx);
//   repo_close(repo);
//   return 0;
// }

int cmd_commit(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    return 0;
}