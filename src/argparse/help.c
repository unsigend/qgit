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

#include "argparse.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LPAD 2
#define MAXW 80
#define BUFSZ 128
#define DESC_GAP 2

static void printwrap(const char *text, int indent, int width)
{
  const char *p = text ? text : "";
  int col = 0;

  for (int i = 0; i < indent; i++) {
    fputc(' ', stdout);
    col++;
  }

  while (*p) {
    while (*p == ' ')
      p++;
    if (!*p)
      break;

    const char *word = p;
    while (*p && *p != ' ')
      p++;
    int wlen = (int)(p - word);

    if (col > indent && col + 1 + wlen > width) {
      fputc('\n', stdout);
      col = 0;
      for (int i = 0; i < indent; i++) {
        fputc(' ', stdout);
        col++;
      }
    } else if (col > indent) {
      fputc(' ', stdout);
      col++;
    }

    fwrite(word, 1, (size_t)wlen, stdout);
    col += wlen;
  }

  fputc('\n', stdout);
}

static void fmtopt(const struct argparse_opt *opt, char *buf, size_t bufsz)
{
  const int hasvalue = (opt->type != _OPT_BOOL) && (opt->type != _OPT_GROUP);
  const char *suffix = hasvalue ? "=<value>" : "";

  if (opt->sname && opt->lname)
    snprintf(buf, bufsz, "-%c, --%s%s", opt->sname, opt->lname, suffix);
  else if (opt->lname)
    snprintf(buf, bufsz, "--%s%s", opt->lname, suffix);
  else if (opt->sname)
    snprintf(buf, bufsz, "-%c%s", opt->sname, hasvalue ? " <value>" : "");
  else
    buf[0] = '\0';
}

static int getoptwidth(struct argparse_opt *opts)
{
  int maxw = 0;
  char spec[BUFSZ];

  for (struct argparse_opt *opt = opts; opt && opt->type != _OPT_END; opt++) {
    if (opt->type == _OPT_GROUP)
      continue;
    fmtopt(opt, spec, sizeof(spec));
    int w = (int)strlen(spec);
    if (w > maxw)
      maxw = w;
  }

  return maxw;
}

static void printopt(const struct argparse_opt *opt, int colw)
{
  char spec[BUFSZ];
  fmtopt(opt, spec, sizeof(spec));

  fprintf(stdout, "%*s%-*s", LPAD, "", colw, spec);

  const char *help = opt->help ? opt->help : "";
  int desc_indent = LPAD + colw + DESC_GAP;
  int col = LPAD + colw;
  for (int i = 0; i < DESC_GAP; i++) {
    fputc(' ', stdout);
    col++;
  }

  const char *p = help;
  while (*p) {
    while (*p == ' ')
      p++;
    if (!*p)
      break;

    const char *word = p;
    while (*p && *p != ' ')
      p++;
    int wlen = (int)(p - word);

    if (col > desc_indent && col + 1 + wlen > MAXW) {
      fputc('\n', stdout);
      col = 0;
      for (int i = 0; i < desc_indent; i++) {
        fputc(' ', stdout);
        col++;
      }
    } else if (col > desc_indent) {
      fputc(' ', stdout);
      col++;
    }

    fwrite(word, 1, (size_t)wlen, stdout);
    col += wlen;
  }

  fputc('\n', stdout);
}

static void printoptions(struct argparse *ctx)
{
  if (!ctx || !ctx->opts)
    return;

  int colw = getoptwidth(ctx->opts);

  fprintf(stdout, "OPTIONS:\n\n");
  for (struct argparse_opt *opt = ctx->opts; opt->type != _OPT_END; opt++) {
    if (opt->type == _OPT_GROUP) {
      if (opt->help && *opt->help)
        fprintf(stdout, "%s:\n", opt->help);
      continue;
    } else if (opt->type == _OPT_GROUP_END) {
      fputc('\n', stdout);
      continue;
    }
    printopt(opt, colw);
  }
}

void argparse_cb_help(struct argparse *ctx, struct argparse_opt *opt)
{
  (void)opt;
  if (!ctx)
    return;

  struct argparse_desc *desc = ctx->desc;
  if (desc) {
    if (desc->desc) {
      fprintf(stdout, "OVERVIEW: ");
      printwrap(desc->desc, 0, MAXW);
      fputc('\n', stdout);
    }

    if (desc->usage) {
      fprintf(stdout, "USAGE: %s\n", desc->usage);
      fputc('\n', stdout);
    }

    if (ctx->opts)
      printoptions(ctx);

    if (ctx->opts)
      fputc('\n', stdout);

    if (desc->epilog)
      printwrap(desc->epilog, 0, MAXW);
  }
  exit(EXIT_SUCCESS);
}