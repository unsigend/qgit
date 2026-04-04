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

#ifndef UTEST_PRINT_H
#define UTEST_PRINT_H

#include <stddef.h>
#include <stdio.h>
#include <time.h>

/* This file is used to manage context unaware output buffer, the operations
   based on the buffer and has no context related to the core. */

struct ut_stats {
  struct timespec start;
  struct timespec end;
  size_t cnpassed;
  size_t cnfailed;
  size_t cnskipped;
  size_t snpassed;
  size_t snfailed;
  size_t snskipped;
};

struct utbuf {
  char *buf;
  size_t bufsz;
  size_t bufcap;
};

#ifdef __cplusplus
extern "C" {
#endif

extern void utbuf_init(struct utbuf *buf, size_t cap);
extern void utbuf_fini(struct utbuf *buf);
extern void utbuf_flush(struct utbuf *buf, FILE *stream);
extern void utbuf_printb(struct utbuf *buf, int type, const char *name);
extern void utbuf_printe(struct utbuf *buf, int type, const char *name,
                         int stat);
extern void utbuf_printstats(struct utbuf *buf, struct ut_stats *stats);
extern void utbuf_printassert(struct utbuf *buf, int line, const char *file,
                              const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif