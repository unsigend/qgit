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

#include "argparse.h"
#include "cmd.h"
#include "error.h"
#include "iniparse.h"

#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

static struct iniFILE *open_global(void)
{
  char path[PATH_MAX];
  char *home = getenv("HOME");
  if (!home)
    return NULL;
  snprintf(path, PATH_MAX, "%s/.qgitconfig", home);
  struct stat st;
  if (stat(path, &st) == -1 || !S_ISREG(st.st_mode))
    return iniparse_create(path);
  struct iniFILE *config = iniparse_open(path);
  if (config && iniparse_parse(config) == -1) {
    iniparse_close(config);
    return NULL;
  }
  return config;
}

static struct iniFILE *open_local(void)
{
  char path[PATH_MAX];
  getcwd(path, PATH_MAX);
  snprintf(path, PATH_MAX, "%s/.qgit/config", path);
  struct iniFILE *config = iniparse_open(path);
  if (config && iniparse_parse(config) == -1) {
    iniparse_close(config);
    return NULL;
  }
  return config;
}

static void splitkey(char *arg, char **sec, char **key)
{
  *sec = arg;
  *key = strchr(arg, '.');
  if (!*key)
    error("qgit: key must be in section.key format\n");
  *(*key)++ = '\0';
}

#define IS_AUTO(l, g) ((!(l) && !(g)))

int cmd_config(int argc, char **argv)
{
  bool global, local;
  bool list;
  bool get, set, unset;
  char *key, *val, *sec;

  global = local = list = false;
  get = set = unset = false;
  key = val = sec = NULL;

  struct argparse ctx;
  struct argparse_opt opts[] = {
      OPT_HELP(),
      OPT_GROUP("Scope"),
      OPT_BOOL(0, "global", "use global config file", &global),
      OPT_BOOL(0, "local", "use local config file", &local),
      OPT_GROUP_END(),
      OPT_GROUP("Action"),
      OPT_BOOL('l', "list", "list all options", &list),
      OPT_BOOL('g', "get", "get the value of a key", &get),
      OPT_BOOL('s', "set", "set the value of a key", &set),
      OPT_BOOL('u', "unset", "unset a key", &unset),
      OPT_GROUP_END(),
      OPT_END(),
  };
  struct argparse_desc desc = {
      .prog = "qgit",
      .desc = "Get and set repository or global options",
      .usage = "qgit config [options]",
      .epilog = "See 'qgit config --help' for more information.",
  };

  argparse_init(&ctx, opts, &desc);
  if (argparse_parse(&ctx, argc, argv) == -1)
    error("qgit: %s\n", argparse_strerror(&ctx));

  struct iniFILE *lcfg = NULL;
  struct iniFILE *gcfg = NULL;

  if (local || IS_AUTO(local, global))
    lcfg = open_local();
  if (local && !lcfg)
    error("qgit: --local can only be used inside a qgit repository\n");
  if (global || IS_AUTO(local, global))
    gcfg = open_global();

  if (list) {
    if (gcfg)
      iniparse_fprint(gcfg, stdout);
    if (lcfg)
      iniparse_fprint(lcfg, stdout);
  } else if (set) {
    if (argparse_getremargc(&ctx) != 2)
      error("qgit: --set requires two arguments\n");
    splitkey(argparse_getremargv(&ctx)[0], &sec, &key);
    val = argparse_getremargv(&ctx)[1];

    struct iniFILE *wtarget = global ? gcfg : lcfg;
    if (!wtarget)
      error("qgit: not inside a qgit repository\n");
    iniparse_set(wtarget, sec, key, val);
    iniparse_write(wtarget);
  } else if (unset) {
    if (argparse_getremargc(&ctx) != 1)
      error("qgit: --unset requires one argument\n");
    splitkey(argparse_getremargv(&ctx)[0], &sec, &key);

    struct iniFILE *wtarget = global ? gcfg : lcfg;
    if (!wtarget)
      error("qgit: not inside a qgit repository\n");
    iniparse_unset(wtarget, sec, key);
    iniparse_write(wtarget);
  } else if (get) {
    if (argparse_getremargc(&ctx) != 1)
      error("qgit: --get requires one argument\n");
    splitkey(argparse_getremargv(&ctx)[0], &sec, &key);

    const char *v = NULL;
    if (global)
      v = iniparse_get(gcfg, sec, key);
    else if (local)
      v = iniparse_get(lcfg, sec, key);
    else {
      v = lcfg ? iniparse_get(lcfg, sec, key) : NULL;
      if (!v)
        v = gcfg ? iniparse_get(gcfg, sec, key) : NULL;
    }
    if (!v) {
      iniparse_close(lcfg);
      iniparse_close(gcfg);
      argparse_fini(&ctx);
      return EXIT_FAILURE;
    }
    printf("%s\n", v);
  } else {
    error("qgit: no action specified\n");
  }

  iniparse_close(lcfg);
  iniparse_close(gcfg);

  argparse_fini(&ctx);
  return 0;
}