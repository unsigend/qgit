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

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <utest/flags.h>
#include <utest/print.h>

#define COLYELLOW "\033[33m"
#define COLBLUE "\033[34m"
#define COLGREEN "\033[32m"
#define COLRED "\033[31m"
#define COLRESET "\033[0m"
#define COLMAGENTA "\033[35m"
#define COLCYAN "\033[36m"

#define MAX(a, b) ((a) > (b) ? (a) : (b))

static void fatal()
{
  perror("utest");
  exit(EXIT_FAILURE);
}

static double elapsedms(struct timespec *start, struct timespec *end)
{
  /* calculate elapsed time in milliseconds */
  return (end->tv_sec - start->tv_sec) * 1000.0 +
         (end->tv_nsec - start->tv_nsec) / 1000000.0;
}

static void growbuf(struct utbuf *buf, size_t newcap)
{
  buf->bufcap = newcap;
  buf->buf = realloc(buf->buf, buf->bufcap);
  if (!buf->buf)
    fatal();
}

static void vprintfbuf(struct utbuf *buf, const char *fmt, va_list ap)
{
  va_list ap2;
  va_copy(ap2, ap);
  int nw = vsnprintf(NULL, 0, fmt, ap2); /* dry run */
  va_end(ap2);

  if (nw < 0)
    fatal();

  if ((size_t)nw > buf->bufcap - buf->bufsz) {
    size_t needed = buf->bufsz + (size_t)nw + 1;
    size_t newcap = MAX(buf->bufcap * 2, needed);
    growbuf(buf, newcap);
  }

  nw = vsnprintf(buf->buf + buf->bufsz, buf->bufcap - buf->bufsz, fmt, ap);
  buf->bufsz += nw;
}

static void printfbuf(struct utbuf *buf, const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  vprintfbuf(buf, fmt, ap);
  va_end(ap);
}

void utbuf_init(struct utbuf *buf, size_t cap)
{
  buf->buf = malloc(cap);
  if (!buf->buf)
    fatal();
  buf->bufsz = 0;
  buf->bufcap = cap;
}

void utbuf_fini(struct utbuf *buf)
{
  free(buf->buf);
  memset(buf, 0, sizeof(*buf));
}

void utbuf_flush(struct utbuf *buf, FILE *stream)
{
  fwrite(buf->buf, 1, buf->bufsz, stream);
  fflush(stream);
}

void utbuf_printb(struct utbuf *buf, int type, const char *name)
{
  if (type == UT_CASE)
    printfbuf(buf, " |- ");
  printfbuf(buf, "[" COLCYAN "RUNNING" COLRESET "] TEST %s: %s\n",
            type == UT_CASE ? "CASE" : "SUITE", name);
}

void utbuf_printe(struct utbuf *buf, int type, const char *name, int stat)
{
  if (type == UT_CASE)
    printfbuf(buf, " |- ");
  printfbuf(buf, "[%s%s%s ] TEST %s: %s\n", stat == UT_PASS ? COLGREEN : COLRED,
            stat == UT_PASS ? "PASSED" : "FAILED", COLRESET,
            type == UT_CASE ? "CASE" : "SUITE", name);
}

void utbuf_printstats(struct utbuf *buf, struct ut_stats *stats)
{
  size_t cntotal = stats->cnpassed + stats->cnfailed + stats->cnskipped;
  size_t sntotal = stats->snpassed + stats->snfailed + stats->snskipped;
  double elapsed = elapsedms(&stats->start, &stats->end);
  printfbuf(buf, "\n");
  printfbuf(buf,
            "[CASES ] skipped: %zu/%zu, failed: %zu/%zu, passed: %zu/%zu\n",
            stats->cnskipped, cntotal, stats->cnfailed, cntotal,
            stats->cnpassed, cntotal);
  printfbuf(buf,
            "[SUITES] skipped: %zu/%zu, failed: %zu/%zu, passed: %zu/%zu\n",
            stats->snskipped, sntotal, stats->snfailed, sntotal,
            stats->snpassed, sntotal);
  if (elapsed > 1000.0)
    printfbuf(buf, "time:  %.2f s\n", elapsed / 1000.0);
  else
    printfbuf(buf, "time:  %.2f ms\n", elapsed);
}

void utbuf_printassert(struct utbuf *buf, int line, const char *file,
                       const char *fmt, ...)
{
  printfbuf(buf, "   |- %s:%d assertion failed\n", file, line);
  printfbuf(buf, "   |  ");
  va_list ap;
  va_start(ap, fmt);
  vprintfbuf(buf, fmt, ap);
  va_end(ap);
  printfbuf(buf, "\n");
}