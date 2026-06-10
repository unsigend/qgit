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

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "argparse.h"
#include "config.h"
#include "die.h"
#include "iniparse.h"

#define IS_AUTO(g, l) (!(l) && !(g))

static int parse(char *parm, char **sec, char **key)
{
  if (!parm)
    return -1;

  char *dot = strchr(parm, '.');
  if (!dot)
    return -1;

  *dot = '\0';
  *sec = parm;
  *key = dot + 1;
  while (**key && isspace(**key))
    (*key)++;
  return 0;
}

/*
  qgit config behaviour:
    --list: if --global or --local is specified, list the global or local config
            file, if auto, list global first then local config.
    --get:  if --global or --local is specified, get the value of the key from
            the global or local config file, if auto, get the value from local
            config first, then from global config if not found.
    --set:  if --global or --local is specified, set the value of the key in the
            global or local config file, if auto, set the value in local config.
    --unset: if --global or --local is specified, unset the value of the key in
            the global or local config file, if auto, unset the value in local
            config.
*/
int cmd_config(int argc, char **argv)
{
  int g, l;
  int list, get, set, unset;
  g = l = list = get = set = unset = 0;
  struct iniFILE *gcfg = NULL, *lcfg = NULL;
  char *sec = NULL, *key = NULL, *value = NULL;
  int ret = EXIT_SUCCESS;

  struct argparse ctx;
  struct argparse_opt opts[] = {
      OPT_GROUP("Scope"),
      OPT_BOOL(0, "global", "use global config file", &g),
      OPT_BOOL(0, "local", "use local config file", &l),
      OPT_GROUP_END(),
      OPT_GROUP("Action"),
      OPT_BOOL('l', "list", "list all options", &list),
      OPT_BOOL('g', "get", "get the value of an key", &get),
      OPT_BOOL('s', "set", "set the value of an key", &set),
      OPT_BOOL('u', "unset", "unset the value of an key", &unset),
      OPT_GROUP_END(),
      OPT_HELP(),
      OPT_END(),
  };

  struct argparse_desc desc = {
      .prog = "qgit config",
      .desc = "Get and set repository or global options",
      .usage = "qgit config [options]",
      .epilog = "See 'qgit config --help' for more information.",
  };

  if (argparse_init(&ctx, opts, &desc) == -1)
    die("%s", ctx.errstr);

  if (argparse_parse(&ctx, argc, argv) == -1)
    die("%s", ctx.errstr);

  if (l && g)
    die("--local and --global cannot be used together");

  if (l || IS_AUTO(g, l))
    lcfg = config_cwd();
  if (l && !lcfg) /* Explicit --local but no local config file */
    die("--local can only be used inside a qgit repository");
  if (g || IS_AUTO(g, l))
    gcfg = config_global();

  if (list) {
    if (IS_AUTO(g, l)) {
      if (gcfg)
        iniparse_fprint(gcfg, stdout);
      if (lcfg)
        iniparse_fprint(lcfg, stdout);
    } else if (g) {
      if (gcfg)
        iniparse_fprint(gcfg, stdout);
    } else if (l) {
      if (lcfg)
        iniparse_fprint(lcfg, stdout);
    }
  } else if (get) {
    if (argparse_getremargc(&ctx) < 1)
      die("--get requires a key");
    if (parse(argparse_getremargv(&ctx)[0], &sec, &key) == -1)
      die("invalid key format");

    if (l) {
      if (lcfg)
        value = (char *)iniparse_get(lcfg, sec, key);
    } else if (g) {
      if (gcfg)
        value = (char *)iniparse_get(gcfg, sec, key);
    } else {
      if (lcfg)
        value = (char *)iniparse_get(lcfg, sec, key);
      if (!value && gcfg)
        value = (char *)iniparse_get(gcfg, sec, key);
    }

    if (value)
      printf("%s\n", value);
    else
      ret = EXIT_FAILURE;

  } else if (set) {
    if (argparse_getremargc(&ctx) < 2)
      die("--set requires a key and a value");
    if (parse(argparse_getremargv(&ctx)[0], &sec, &key) == -1)
      die("invalid key format");

    value = argparse_getremargv(&ctx)[1];
    if (g) {
      if (!gcfg) {
        char path[PATH_MAX];
        if (config_global_path(path) == -1)
          die_errno();
        gcfg = iniparse_create(path);
        if (!gcfg)
          die_errno();
      }
      if (iniparse_set(gcfg, sec, key, value) == -1)
        die_errno();
      if (iniparse_write(gcfg) == -1)
        die_errno();
    } else {
      /* local config file must exist here */
      if (!lcfg)
        die("qgit repository may broken");
      if (iniparse_set(lcfg, sec, key, value) == -1)
        die_errno();
      if (iniparse_write(lcfg) == -1)
        die_errno();
    }

  } else if (unset) {
    if (argparse_getremargc(&ctx) < 1)
      die("--unset requires a key");
    if (parse(argparse_getremargv(&ctx)[0], &sec, &key) == -1)
      die("invalid key format");

    if (g) {
      if (gcfg) {
        if (iniparse_unset(gcfg, sec, key) == -1)
          ret = EXIT_FAILURE;
        else if (iniparse_write(gcfg) == -1)
          die_errno();
      }
    } else {
      if (lcfg) {
        if (iniparse_unset(lcfg, sec, key) == -1)
          ret = EXIT_FAILURE;
        else if (iniparse_write(lcfg) == -1)
          die_errno();
      }
    }
  } else
    die("no action specified");

  iniparse_close(gcfg);
  iniparse_close(lcfg);
  argparse_fini(&ctx);
  return ret;
}