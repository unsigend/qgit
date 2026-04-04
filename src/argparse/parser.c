/**
 * MIT License
 *
 * Copyright (c) 2026 YIXIANG QIU
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <argparse.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OPT 0
#define ARG 1
#define NLIST 8
#define FACTOR 2
#define BUFSZ 256

#define getstate(s) (s[0] == '-' ? OPT : ARG)

#define IS_OPTARG(f) ((f) & OPT_OPTIONAL)
#define IS_REQARG(f) ((f) & OPT_REQUIRED)
#define IS_NONARG(f) ((f) & OPT_NONE)
#define IS_IGNERROR(f) ((f) & ARG_IGNORE)
#define has_next(p, end) ((p) + 1 < (end))

/* Write value to destination, if dest is NULL, dry run the value */
static int dumpvalue(void *dest, int type, const char *arg);
static int dumperror(struct argparse *ctx, const char *fmt, ...);

/* Find an option in list by short name or long name */
static struct argparse_opt *findopt(struct argparse *ctx, char s,
                                    const char *l);

/* Get a slot and push the value to the list, write back the list pointer to the
destination pointer */
static inline int push(struct argparse *ctx, void *dest, char *p);

/* Get or allocate one slot in the list array */
static struct argparse_list *getslot(struct argparse *ctx);
/* Add one argument to the argument list */
static int arglist_add(struct argparse_list *list, char *arg);

/* Options parsing functions */
static char **parse_short(struct argparse *ctx, char **p, char **end);
static char **parse_long(struct argparse *ctx, char **p, char **end);

int argparse_init(struct argparse *ctx, struct argparse_opt *opts,
                  struct argparse_desc *desc)
{
  if (!ctx || !opts)
    return -1;

  memset(ctx, 0, sizeof(struct argparse));
  ctx->opts = opts;
  ctx->desc = desc;

  dumperror(ctx, NULL);

  return 0;
}

void argparse_fini(struct argparse *ctx)
{
  if (!ctx)
    return;

  if (ctx->lists) {
    for (size_t i = 0; i < ctx->nlists; i++) {
      if (ctx->lists[i] && ctx->lists[i]->items)
        free(ctx->lists[i]->items);
      free(ctx->lists[i]);
    }
    free(ctx->lists);
  }
  if (ctx->remlist.items)
    free(ctx->remlist.items);
  if (ctx->errstr)
    free(ctx->errstr);
}

int argparse_parse(struct argparse *ctx, int argc, char **argv)
{
  if (!ctx)
    return -1;
  if (!argc || !argv)
    return 0;

  char **cur = argv;
  int stop = 0;
  int state = getstate(*cur);
  char **ep = argv + argc;

  while (cur < ep) {
    switch (state) {
    case OPT:
      if ((*cur)[1] == '-') {
        if (strcmp(*cur, "--") == 0) {
          /* stop option parsing */
          cur++;
          state = ARG;
          stop = 1;
        } else {
          /* long option */
          cur = parse_long(ctx, cur, ep);
          if (!cur)
            return -1;
          if (cur < ep)
            state = getstate(*cur);
        }
      } else {
        /* short option */
        cur = parse_short(ctx, cur, ep);
        if (!cur)
          return -1;
        if (cur < ep)
          state = getstate(*cur);
      }
      break;
    case ARG:
      if (arglist_add(&ctx->remlist, *cur) == -1)
        return -1;
      cur++;
      if (!stop && cur < ep)
        state = getstate(*cur);
      break;
    }
  }

  return 0;
}

static struct argparse_opt *findopt(struct argparse *ctx, char s, const char *l)
{
  struct argparse_opt *opt = ctx->opts;
  while (opt->type != _OPT_END) {
    if ((s && opt->sname == s) ||
        (l && opt->lname && strcmp(opt->lname, l) == 0))
      return opt;
    opt++;
  }
  return NULL;
}

static struct argparse_list *getslot(struct argparse *ctx)
{
  if (!ctx->lists) {
    ctx->lists = calloc(NLIST, sizeof(struct argparse_list *));
    if (!ctx->lists)
      return NULL;
    ctx->nlistscap = NLIST;
  } else if (ctx->nlists >= ctx->nlistscap) {
    size_t newsz = ctx->nlistscap * FACTOR;
    void *p = realloc(ctx->lists, newsz * sizeof(struct argparse_list *));
    if (!p)
      return NULL;
    ctx->lists = p;
    ctx->nlistscap = newsz;
  }
  struct argparse_list *slot = calloc(1, sizeof(struct argparse_list));
  if (!slot)
    return NULL;
  ctx->lists[ctx->nlists++] = slot;
  return slot;
}

static int arglist_add(struct argparse_list *list, char *arg)
{
  if (!list->items) {
    list->items = calloc(NLIST, sizeof(char *));
    if (!list->items)
      return -1;
    list->cap = NLIST;
  } else if (list->sz >= list->cap) {
    size_t newsz = list->cap * FACTOR;
    void *p = realloc(list->items, newsz * sizeof(char *));
    if (!p)
      return -1;
    list->items = p;
    list->cap = newsz;
  }
  list->items[list->sz++] = arg;
  return 0;
}

static int dumperror(struct argparse *ctx, const char *fmt, ...)
{
  if (!ctx->errstr) {
    ctx->errstr = malloc(BUFSZ);
    if (!ctx->errstr)
      return -1;
    ctx->errstr[0] = '\0';
  }
  if (!fmt)
    return 0;
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(ctx->errstr, BUFSZ, fmt, ap);
  va_end(ap);
  return 0;
}

static int dumpvalue(void *dest, int type, const char *arg)
{
  errno = 0;
  switch (type) {
  case _OPT_BOOL:
    if (dest)
      *(bool *)dest = true;
    break;
  case _OPT_INT: {
    if (!arg || !*arg)
      return -1;
    char *endptr = NULL;
    long v = strtol(arg, &endptr, 0);
    if (errno == ERANGE || errno == EINVAL || *endptr != '\0' || v > INT_MAX ||
        v < INT_MIN) /* integer overflow or underflow */
      return -1;
    if (dest)
      *(int *)dest = (int)v;
    break;
  }
  case _OPT_STR:
    if (dest)
      *(char **)dest = (char *)arg;
    break;
  case _OPT_LONG: {
    if (!arg || !*arg)
      return -1;
    char *endptr = NULL;
    long v = strtol(arg, &endptr, 0);
    if (errno == ERANGE || errno == EINVAL || *endptr != '\0')
      return -1;
    if (dest)
      *(long *)dest = v;
    break;
  }
  case _OPT_DOUBLE: {
    if (!arg || !*arg)
      return -1;
    char *endptr = NULL;
    double v = strtod(arg, &endptr);
    if (errno == ERANGE || errno == EINVAL || *endptr != '\0')
      return -1;
    if (dest)
      *(double *)dest = v;
    break;
  }
  default:
    return -1;
  }
  return 0;
}

static inline int push(struct argparse *ctx, void *dest, char *p)
{
  struct argparse_list **listp = (struct argparse_list **)dest;
  if (!*listp) {
    *listp = getslot(ctx);
    if (!*listp)
      return -1;
  }
  return arglist_add(*listp, p);
}

static char **parse_short(struct argparse *ctx, char **p, char **end)
{
  const char *s = *p + 1;
  /* handle combined options -abc */
  while (*s) {
    struct argparse_opt *opt = findopt(ctx, *s, NULL);
    if (!opt) {
      if (!IS_IGNERROR(ctx->flags)) {
        dumperror(ctx, "unknown option: '-%c'", *s);
        return NULL;
      }
      s++;
      continue;
    }

    if (opt->dest) {
      if (opt->type == _OPT_BOOL)
        dumpvalue(opt->dest, _OPT_BOOL, NULL);
      else {
        if (!IS_NONARG(opt->flags)) {
          const char *vp = NULL;
          int usenext = 0;
          char optchar = *s;

          if (*(s + 1)) { /* -j6 */
            vp = s + 1;
            s += strlen(s) - 1;
          } else if (has_next(p, end)) { /* -j 6 */
            vp = *(p + 1);
            p++;
            usenext = 1;
          }

          if (opt->type == _OPT_LIST) {
            if (!vp) {
              dumperror(ctx, "missing value for option: '-%c'", optchar);
              return NULL;
            }
            if (push(ctx, opt->dest, (char *)vp) == -1)
              return NULL;
          } else {
            int r = dumpvalue(NULL, opt->type, vp);
            if (IS_REQARG(opt->flags)) {
              if (r == -1) {
                if (!vp)
                  dumperror(ctx, "missing value for option: '-%c'", optchar);
                else
                  dumperror(ctx, "invalid value for option: '-%c'", optchar);
                return NULL;
              }
              dumpvalue(opt->dest, opt->type, vp);
            } else {
              if (r != -1)
                dumpvalue(opt->dest, opt->type, vp);
              else if (usenext)
                --p;
            }
          }
        }
      }
    }
    s++;
    if (opt->cb)
      opt->cb(ctx, opt);
  }
  return p + 1;
}

static char **parse_long(struct argparse *ctx, char **p, char **end)
{
  const char *s = *p + 2;
  char *eq = strchr(s, '=');
  char buf[BUFSZ];
  buf[0] = '\0';
  if (eq) {
    memcpy(buf, s, eq - s);
    buf[eq - s] = '\0';
  }
  struct argparse_opt *opt = findopt(ctx, 0, eq ? buf : s);
  if (!opt) {
    if (!IS_IGNERROR(ctx->flags)) {
      dumperror(ctx, "unknown option: '--%s'", s);
      return NULL;
    }
    return p + 1;
  }

  if (opt->dest) {
    if (opt->type == _OPT_BOOL)
      dumpvalue(opt->dest, _OPT_BOOL, NULL);
    else {
      if (!IS_NONARG(opt->flags)) {
        const char *vp = NULL;
        int usenext = 0;
        if (eq && eq[1]) /* --opt=value */
          vp = eq + 1;
        else if (has_next(p, end)) /* --opt value */
        {
          vp = *(p + 1);
          p++;
          usenext = 1;
        }

        if (opt->type == _OPT_LIST) {
          if (!vp) {
            dumperror(ctx, "missing value for option: '--%s'",
                      buf[0] ? buf : s);
            return NULL;
          }
          if (push(ctx, opt->dest, (char *)vp) == -1)
            return NULL;
        } else {
          int r = dumpvalue(NULL, opt->type, vp);
          if (IS_REQARG(opt->flags)) {
            if (r == -1) {
              if (!vp)
                dumperror(ctx, "missing value for option: '--%s'",
                          buf[0] ? buf : s);
              else
                dumperror(ctx, "invalid value for option: '--%s'",
                          buf[0] ? buf : s);
              return NULL;
            }
            dumpvalue(opt->dest, opt->type, vp);
          } else {
            if (r != -1)
              dumpvalue(opt->dest, opt->type, vp);
            else if (usenext)
              --p;
          }
        }
      }
    }
  }

  if (opt->cb)
    opt->cb(ctx, opt);

  return p + 1;
}
