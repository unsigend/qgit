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

#include <stdlib.h>
#include <string.h>

#include "argparse.h"
#include "cmd.h"
#include "config.h"
#include "die.h"
#include "iniparse.h"
#include "repo.h"

static void action_check(int list, int get, int set, int unset)
{
  if ((list && (get || set || unset)) || (get && (set || unset)) ||
      (set && unset))
    die("cannot use multiple actions at once");
}

static void scope_check(int g, int l)
{
  if (g && l)
    die("cannot use --global and --local together");
}

static void parse(char *buf, char **sec, char **key) /* inplace parse */
{
  char *dot = strchr(buf, '.');
  if (!dot || dot == buf || *(dot + 1) == '\0')
    die("invalid key format");
  *dot++ = '\0';
  *sec = buf;
  *key = dot;
  if (!*sec || !*key)
    die("invalid key format");
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

struct config_flags {
  int g, l, a;               /* scope */
  int list, get, set, unset; /* action */
};

int cmd_config(int argc, char **argv)
{
  struct config_flags f = {0};
  struct iniFILE *gcfg = NULL, *lcfg = NULL;
  struct repo *repo = NULL;
  char *key, *val, *sec;
  int ret = 0;

  struct argparse ctx;
  struct argparse_opt opts[] = {
      OPT_HELP(),
      OPT_GROUP("Scope"),
      OPT_BOOL(0, "global", "use global config file", &f.g),
      OPT_BOOL(0, "local", "use local config file", &f.l),
      OPT_GROUP_END(),
      OPT_GROUP("Action"),
      OPT_BOOL('l', "list", "list all options", &f.list),
      OPT_BOOL('g', "get", "get the value of an key", &f.get),
      OPT_BOOL('s', "set", "set the value of an key", &f.set),
      OPT_BOOL('u', "unset", "unset the value of an key", &f.unset),
      OPT_GROUP_END(),
      OPT_END(),
  };

  static const char *usages[] = {
      "qgit config [<scope>] --list",
      "qgit config [<scope>] --get <key>",
      "qgit config [<scope>] --set <key> <value>",
      "qgit config [<scope>] --unset <key>",
  };

  struct argparse_desc desc = {
      .prog = "qgit config",
      .desc = "Get and set repository or global options",
      .usages = usages,
      .nusages = sizeof(usages) / sizeof(usages[0]),
      .epilog = "key format: <section>.<name>",
  };

  if (argparse_init(&ctx, opts, &desc) == -1)
    die("%s", argparse_strerror(&ctx));
  if (argparse_parse(&ctx, argc, argv) == -1)
    die("%s", argparse_strerror(&ctx));

  if (!f.list && !f.get && !f.set && !f.unset)
    die("no action specified");

  action_check(f.list, f.get, f.set, f.unset);
  scope_check(f.g, f.l);
  f.a = !f.g && !f.l;

  repo = repo_findcwd();

  if (f.g || f.a)
    gcfg = config_global();

  if (f.l && !repo)
    die("not inside a qgit repository");
  if (f.l || f.a)
    lcfg = config_repo(repo);
  if (f.l && !lcfg)
    die("missing config file in qgit repository");

  if (f.list) /* list mode */
  {
    if (f.g) {
      if (!gcfg) {
        if (!getenv("HOME"))
          die("HOME is not set");
        ret = 1;
      } else
        iniparse_fprint(gcfg, stdout);
    } else if (f.l) {
      iniparse_fprint(lcfg, stdout);
    } else if (f.a) {
      if (gcfg)
        iniparse_fprint(gcfg, stdout);
      if (lcfg)
        iniparse_fprint(lcfg, stdout);
    }
  } else if (f.get) /* get mode */
  {
    if (argparse_getremargc(&ctx) < 1)
      die("--get requires a key");
    val = NULL;
    parse(argparse_getremargv(&ctx)[0], &sec, &key);

    if (f.g) {
      if (gcfg) {
        val = (char *)iniparse_get(gcfg, sec, key);
        if (val)
          printf("%s\n", val);
        else
          ret = 1;
      } else
        ret = 1;
    } else if (f.l) {
      val = (char *)iniparse_get(lcfg, sec, key);
      if (val)
        printf("%s\n", val);
      else
        ret = 1;
    } else if (f.a) {
      int found = 0;
      if (lcfg) {
        val = (char *)iniparse_get(lcfg, sec, key);
        if (val) {
          printf("%s\n", val);
          found = 1;
        }
      }
      if (gcfg && !found) {
        val = (char *)iniparse_get(gcfg, sec, key);
        if (val) {
          printf("%s\n", val);
          found = 1;
        }
      }
      if (!found)
        ret = 1;
    }
  } else if (f.set) /* set mode */
  {
    if (argparse_getremargc(&ctx) < 2)
      die("--set requires a key and a value");
    parse(argparse_getremargv(&ctx)[0], &sec, &key);
    val = argparse_getremargv(&ctx)[1];

    if (f.g) {
      if (gcfg) {
        if (iniparse_set(gcfg, sec, key, val) == -1)
          die_errno();
        if (iniparse_write(gcfg) == -1)
          die_errno();
      } else /* create global config file */
      {
        gcfg = config_global_create();
        if (!gcfg)
          die_errno();
        if (iniparse_set(gcfg, sec, key, val) == -1)
          die_errno();
        if (iniparse_write(gcfg) == -1)
          die_errno();
      }
    } else {
      if (f.a && !repo)
        die("not inside a qgit repository");
      if (!lcfg)
        die("missing config file in qgit repository");

      if (iniparse_set(lcfg, sec, key, val) == -1)
        die_errno();
      if (iniparse_write(lcfg) == -1)
        die_errno();
    }

  } else if (f.unset) /* unset mode */
  {
    if (argparse_getremargc(&ctx) < 1)
      die("--unset requires a key");
    parse(argparse_getremargv(&ctx)[0], &sec, &key);

    if (f.g) {
      if (gcfg) {
        if (iniparse_unset(gcfg, sec, key) == -1)
          ret = 1;
        else if (iniparse_write(gcfg) == -1)
          die_errno();
      } else
        ret = 1;
    } else {
      if (f.a && !repo)
        die("not inside a qgit repository");
      if (!lcfg)
        die("missing config file in qgit repository");

      else {
        if (iniparse_unset(lcfg, sec, key) == -1)
          ret = 1;
        else if (iniparse_write(lcfg) == -1)
          die_errno();
      }
    }
  }

  iniparse_close(gcfg);
  iniparse_close(lcfg);
  repo_close(repo);
  argparse_fini(&ctx);
  return ret;
}