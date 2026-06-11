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
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "argparse.h"
#include "die.h"
#include "obj/obj.h"

#define ASCII_COLOR_YELLOW "\033[33m"
#define ASCII_COLOR_RESET "\033[0m"

#define UNLIMITED -1

struct log_ctx {
  int oneline;
  int n;
};

static int log_cb(struct obj *obj, void *arg)
{
  struct log_ctx *lctx = (struct log_ctx *)arg;
  if (!lctx) {
    errno = EINVAL;
    return -1;
  }

  unsigned char hex[SHA1_HEX_LENGTH];
  int interactive = isatty(fileno(stdout));

  if (lctx->n == 0)
    return 1;

  if (lctx->oneline) {
    if (sha1_to_hex(obj->sha1, hex) == -1)
      return -1;
    printf("%s%.7s%s %s", interactive ? ASCII_COLOR_YELLOW : "", (char *)hex,
           interactive ? ASCII_COLOR_RESET : "", obj->commit.message);
  } else {
    if (sha1_to_hex(obj->sha1, hex) == -1)
      return -1;

    char *zone = NULL;
    char *timestamp = NULL;
    /* Copy the author to heap and modify inplace */
    char *buf = strdup(obj->commit.author);
    if (!buf)
      return -1;
    char *cursor = buf + strlen(buf) - 1;
    while (cursor > buf && *cursor != ' ')
      cursor--;
    *cursor = '\0';
    zone = cursor + 1;
    while (cursor > buf && *cursor != ' ')
      cursor--;
    *cursor = '\0';
    timestamp = cursor + 1;

    char *endptr = NULL;
    errno = 0;
    time_t t = (time_t)strtol(timestamp, &endptr, 10);
    if (*endptr != '\0' || errno != 0) {
      free(buf);
      return -1;
    }
    struct tm tm = {0};
    localtime_r(&t, &tm);
    char datebuf[64];
    strftime(datebuf, sizeof(datebuf), "%a %b %e %H:%M:%S %Y", &tm);
    printf("%scommit %s%s\n", interactive ? ASCII_COLOR_YELLOW : "",
           (char *)hex, interactive ? ASCII_COLOR_RESET : "");
    printf("Author: %s\n", buf);
    printf("Date:   %s %s\n\n", datebuf, zone);
    printf("    %s", obj->commit.message);
    if (lctx->n != 1)
      printf("\n");
    free(buf);
  }

  if (lctx->n != UNLIMITED)
    lctx->n--;

  return 0;
}

int cmd_log(int argc, char **argv)
{
  struct log_ctx lctx = {
      .oneline = 0,
      .n = UNLIMITED, /* unlimited */
  };
  int first_parent = 0;
  const char *head = "HEAD";

  struct argparse ctx;
  struct argparse_opt opts[] = {
      OPT_HELP(),
      OPT_BOOL(0, "oneline", "show commit logs in one line", &lctx.oneline),
      OPT_BOOL(0, "first-parent", "show only the first parent of each commit",
               &first_parent),
      OPT_INT('n', NULL, "limit the number of commits to show", &lctx.n,
              OPT_REQUIRED),
      OPT_END(),
  };
  struct argparse_desc desc = {
      .prog = "log",
      .desc = "Show commit logs",
      .usage = "qgit log [options]",
  };

  if (argparse_init(&ctx, opts, &desc) == -1)
    die("%s", ctx.errstr);

  if (argparse_parse(&ctx, argc, argv) == -1)
    die("%s", ctx.errstr);

  if (argparse_getremargc(&ctx) > 0)
    head = argparse_getremargv(&ctx)[0];

  struct repo *repo = repo_cwd();
  if (!repo)
    die("not inside a qgit repository");

  unsigned char sha1[SHA1_DIGEST_LENGTH];
  if (obj_resolve(repo, head, sha1) == -1)
    die("ambiguous arguments '%s' unknown reference", head);

  struct obj *obj = obj_open_sha1(repo, sha1);
  if (!obj)
    die_errno();

  if (obj->type != OBJ_COMMIT)
    die("%s not a commit", head);

  if (obj_parse(obj) == -1)
    die_errno();

  commit_walk_type_t type = first_parent ? COMMITWK_FIRST : COMMITWK_ALL;
  if (commit_walk(obj, type, repo, log_cb, &lctx) == -1)
    die_errno();

  obj_close(obj);
  repo_free(repo);
  argparse_fini(&ctx);
  return 0;
}