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

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "argparse.h"
#include "die.h"
#include "index.h"
#include "repo.h"

static int prefix_match(const char *prefix, const char *path)
{
  if (!prefix || !path)
    return 0;
  size_t len = strlen(prefix);
  return strncmp(prefix, path, len) == 0 &&
         (path[len] == '\0' || path[len] == '/');
}

int cmd_ls_files(int argc, char **argv)
{
  int cached = 0;
  int stage = 0;
  const char *path = NULL;

  struct argparse ctx;
  struct argparse_opt opts[] = {
      OPT_HELP(),
      OPT_BOOL('c', "cached", "show cached files", &cached),
      OPT_BOOL('s', "stage", "show staged files", &stage),
      OPT_END(),
  };

  static const char *usages[] = {
      "qgit ls-files [-c --cached] [-s --stage] [<path>]",
  };

  struct argparse_desc desc = {
      .prog = "qgit ls-files",
      .desc = "Show information about files in the index and working tree",
      .usages = usages,
      .nusages = sizeof(usages) / sizeof(usages[0]),
  };

  if (argparse_init(&ctx, opts, &desc) == -1)
    die("%s", argparse_strerror(&ctx));
  if (argparse_parse(&ctx, argc, argv) == -1)
    die("%s", argparse_strerror(&ctx));

  if (argparse_getremargc(&ctx) > 0)
    path = argparse_getremargv(&ctx)[0];

  struct repo *repo = NULL;
  struct index *index = NULL;

  if (!((repo = repo_findcwd())))
    die_errno();

  if (!((index = index_open(repo))))
    die_errno();

  int only_cached = !stage;

  if (stage) {
    for (size_t i = 0; i < vec_size(&index->entries); i++) {
      struct index_entry *entry = vec_at(&index->entries, i);
      if (path && !prefix_match(path, entry->path))
        continue;
      unsigned char hex[SHA1_HEXLEN];
      if (sha1_to_hex(entry->sha1, hex) == -1)
        die_errno();
      printf("%06o %s %u\t%s\n", entry->mode, hex, entry->stage, entry->path);
    }
  } else if (cached || only_cached) {
    for (size_t i = 0; i < vec_size(&index->entries); i++) {
      struct index_entry *entry = vec_at(&index->entries, i);
      if (path && !prefix_match(path, entry->path))
        continue;
      printf("%s\n", entry->path);
    }
  }

  argparse_fini(&ctx);
  index_close(index);
  repo_close(repo);
  return 0;
}