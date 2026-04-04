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

#ifndef ARGPARSE_H
#define ARGPARSE_H

#include <stddef.h>

/* A lightweight command line argument parser, compatible with GNU/POSIX
   option parsing style.

   Single value parsing:
                -f<value> | -f <value> | -abc (for BOOL type)
                --flag<value> | --flag=<value>

   Multiple values parsing:
                -I./include -I./arch -I./src

   flag -- stops option parsing, all following arguments are treated as
   positional arguments. For numeric types, the parser will parse the value and
   write the result to the destination pointer. If error occurs, when ARG_IGNORE
   flag is not set, set errstr and exit.

   Note: For LIST type, caller should initialize the list pointer to NULL, and
   the parser will allocate memory for the list if it is not NULL.
*/

#define _OPT_END 0
#define _OPT_INT 1
#define _OPT_STR 2
#define _OPT_BOOL 3
#define _OPT_LONG 4
#define _OPT_DOUBLE 5
#define _OPT_LIST 6
#define _OPT_GROUP 7
#define _OPT_GROUP_END 8

#define OPT_REQUIRED 0x01 /* require argument */
#define OPT_OPTIONAL 0x02 /* optional argument */
#define OPT_NONE 0x00     /* no argument */

/* ignore unknown flags */
#define ARG_IGNORE 0x01

struct argparse;
struct argparse_desc;
struct argparse_opt;
struct argparse_list;

#define OPT_DECL(t, s, l, h, d, f, c)                                          \
  {.sname = s, .lname = l, .type = t, .dest = d, .help = h, .cb = c, .flags = f}
#define OPT_BOOL(s, l, h, d) OPT_DECL(_OPT_BOOL, s, l, h, d, OPT_NONE, NULL)
#define OPT_INT(s, l, h, d, f) OPT_DECL(_OPT_INT, s, l, h, d, f, NULL)
#define OPT_STR(s, l, h, d, f) OPT_DECL(_OPT_STR, s, l, h, d, f, NULL)
#define OPT_LONG(s, l, h, d, f) OPT_DECL(_OPT_LONG, s, l, h, d, f, NULL)
#define OPT_DOUBLE(s, l, h, d, f) OPT_DECL(_OPT_DOUBLE, s, l, h, d, f, NULL)
#define OPT_LIST(s, l, h, d) OPT_DECL(_OPT_LIST, s, l, h, d, OPT_REQUIRED, NULL)
#define OPT_GROUP(h) OPT_DECL(_OPT_GROUP, '\0', NULL, h, NULL, 0, NULL)
#define OPT_END() OPT_DECL(_OPT_END, '\0', NULL, NULL, NULL, 0, NULL)
#define OPT_GROUP_END()                                                        \
  OPT_DECL(_OPT_GROUP_END, '\0', NULL, NULL, NULL, 0,                          \
           NULL) /* group end '\n' no-op */
#define OPT_HELP()                                                             \
  OPT_DECL(_OPT_BOOL, 'h', "help", "show this help message ", NULL, OPT_NONE,  \
           argparse_cb_help)

#define argparse_setflags(ctx, flag) ((ctx)->flags |= (flag)) /* Set flags */
#define argparse_clrflags(ctx, flag)                                           \
  ((ctx)->flags &= ~(flag)) /* Clear flags                                     \
                             */
#define argparse_getremargc(ctx)                                               \
  ((ctx)->remlist.sz) /* Get positional arguments count */
#define argparse_getremargv(ctx)                                               \
  ((ctx)->remlist.items)                       /* Get positional arguments */
#define argparse_strerror(ctx) ((ctx)->errstr) /* Get error string */
#define argparse_getlist(list) ((list)->items) /* Get list items */
#define argparse_getlistsz(list) ((list)->sz)  /* Get list size */

/* callback function */
typedef void (*argparse_cb)(struct argparse *, struct argparse_opt *);

#ifdef __cplusplus
extern "C" {
#endif

/* Return 0 on success, -1 on error. */
extern int argparse_init(struct argparse *ctx, struct argparse_opt *opts,
                         struct argparse_desc *desc);

/* Parse the command line arguments, no program name is needed. Return 0 on
   success, -1 on error. And set error string to ctx->errstr. */
extern int argparse_parse(struct argparse *ctx, int argc, char **argv);

extern void argparse_fini(struct argparse *ctx);

/* built-in callback functions */
extern void argparse_cb_help(struct argparse *, struct argparse_opt *);

#ifdef __cplusplus
}
#endif

struct argparse_opt {
  char sname;
  const char *lname;
  int type;
  void *dest;
  const char *help;
  argparse_cb cb;
  int flags;
};

struct argparse_desc {
  const char *prog;
  const char *desc;
  const char *usage;
  const char *epilog;
};

struct argparse_list {
  char **items;
  size_t sz;
  size_t cap;
};

struct argparse {
  struct argparse_opt *opts;
  struct argparse_desc *desc;
  char *errstr;
  struct argparse_list remlist;
  struct argparse_list **lists;
  size_t nlists;
  size_t nlistscap;
  int flags;
};

#endif