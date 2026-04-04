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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <utest/core.h>
#include <utest/flags.h>
#include <utest/print.h>
#include <utest/types.h>

#define NSUITES 64
#define FACTOR 2
#define BUFSZ 1024

#define LOCK(lock)                                                             \
  if (pthread_mutex_lock(lock))                                                \
    fatal();
#define UNLOCK(lock)                                                           \
  if (pthread_mutex_unlock(lock))                                              \
    fatal();

struct ut_ctx utest_ctx;

static void fatal()
{
  perror("utest");
  exit(EXIT_FAILURE);
}

static void error(const char *msg)
{
  fprintf(stderr, "utest: %s\n", msg);
  exit(EXIT_FAILURE);
}

void ut_init(int flags)
{
  memset(&utest_ctx, 0, sizeof(utest_ctx));
  atomic_init(&utest_ctx.stop, false);
  atomic_init(&utest_ctx.nextsidx, 0);

  if (!flags)
    utest_ctx.flags = UTF_DEFAULT;
  else
    utest_ctx.flags = flags;
  clock_gettime(CLOCK_MONOTONIC, &utest_ctx.tstart);
  if (pthread_mutex_init(&utest_ctx.lock, NULL))
    fatal();
  /* lazy allocate until first suite is added */
  utest_ctx.nsuitescap = NSUITES;
}

void ut_fini(void)
{
  clock_gettime(CLOCK_MONOTONIC, &utest_ctx.tend);
  struct ut_stats stats = {
      .start = utest_ctx.tstart,
      .end = utest_ctx.tend,
      .cnpassed = utest_ctx.cnpassed,
      .cnfailed = utest_ctx.cnfailed,
      .cnskipped = utest_ctx.cnskipped,
      .snpassed = utest_ctx.snpassed,
      .snfailed = utest_ctx.snfailed,
      .snskipped = utest_ctx.snskipped,
  };

  struct utbuf buf;
  utbuf_init(&buf, BUFSZ);
  utbuf_printstats(&buf, &stats);
  utbuf_flush(&buf, stdout);
  utbuf_fini(&buf);

  if (pthread_mutex_destroy(&utest_ctx.lock))
    fatal();

  if (utest_ctx.suites) {
    for (size_t i = 0; i < utest_ctx.nsuites; i++)
      utbuf_fini(&utest_ctx.suites[i].buf);
    free(utest_ctx.suites);
  }
}

void ut_addsuite(const char *name, utsuite_func func)
{
  if (utest_ctx.suites) {
    if (utest_ctx.nsuites >= utest_ctx.nsuitescap) {
      utest_ctx.nsuitescap *= FACTOR;
      utest_ctx.suites = realloc(utest_ctx.suites,
                                 utest_ctx.nsuitescap * sizeof(struct utsuite));
      if (!utest_ctx.suites)
        fatal();
    }
  } else {
    utest_ctx.suites = calloc(NSUITES, sizeof(struct utsuite));
    if (!utest_ctx.suites)
      fatal();
  }
  memset(&utest_ctx.suites[utest_ctx.nsuites], 0, sizeof(struct utsuite));
  utest_ctx.suites[utest_ctx.nsuites].name = name;
  utest_ctx.suites[utest_ctx.nsuites].func = func;
  utbuf_init(&utest_ctx.suites[utest_ctx.nsuites].buf, BUFSZ);
  utest_ctx.nsuites++;
}

void ut_runcase(struct utsuite *suite, const char *name, utcase_func func)
{
  suite->cntotal++;

  if ((utest_ctx.flags & UTF_STOPONCASE) && suite->cnfailed) {
    suite->cnskipped++;
    return;
  }
  struct utcase cas = {
      .name = name,
      .func = func,
      .status = UT_PASS,
      .buf = &suite->buf,
  };

  if (utest_ctx.flags & UTF_SHOWCASE)
    utbuf_printb(cas.buf, UT_CASE, name);
  func(&cas);

  if (utest_ctx.flags & UTF_SHOWCASE || cas.status == UT_FAIL)
    utbuf_printe(cas.buf, UT_CASE, name, cas.status);

  if (cas.status == UT_PASS)
    suite->cnpassed++;
  else
    suite->cnfailed++;
}

static void runsuite(struct utsuite *suite)
{
  if (!suite->func)
    fatal();

  if ((utest_ctx.flags & UTF_STOPONSUITE) && atomic_load(&utest_ctx.stop)) {
    LOCK(&utest_ctx.lock);
    utest_ctx.snskipped++;
    UNLOCK(&utest_ctx.lock);
    return;
  }

  if (utest_ctx.flags & UTF_SHOWSUITE)
    utbuf_printb(&suite->buf, UT_SUITE, suite->name);

  suite->func(suite);

  if (utest_ctx.flags & UTF_SHOWSUITE || suite->cnfailed)
    utbuf_printe(&suite->buf, UT_SUITE, suite->name,
                 suite->cnfailed ? UT_FAIL : UT_PASS);

  utbuf_flush(&suite->buf, stdout);

  LOCK(&utest_ctx.lock);

  if (suite->cnfailed) {
    utest_ctx.snfailed++;
    if (utest_ctx.flags & UTF_STOPONSUITE)
      atomic_store(&utest_ctx.stop, true);
  } else
    utest_ctx.snpassed++;

  /* update global stats */
  utest_ctx.cnpassed += suite->cnpassed;
  utest_ctx.cnfailed += suite->cnfailed;
  utest_ctx.cnskipped += suite->cnskipped;

  UNLOCK(&utest_ctx.lock);
}

static void *runsuite_thread(void *arg)
{
  (void)arg;
  while (1) {
    size_t sidx = atomic_fetch_add(&utest_ctx.nextsidx, 1);
    if (sidx >= utest_ctx.nsuites)
      break;
    runsuite(&utest_ctx.suites[sidx]);
  }
  return NULL;
}

int ut_runsuite(const char *name)
{
  for (size_t i = 0; i < utest_ctx.nsuites; i++) {
    if (!strcmp(utest_ctx.suites[i].name, name)) {
      runsuite(&utest_ctx.suites[i]);
      return 0;
    }
  }
  return -1;
}

void ut_showsuites(void)
{
#define TTY_WIDTH 80

  size_t maxlen = 0;
  for (size_t i = 0; i < utest_ctx.nsuites; i++) {
    size_t len = strlen(utest_ctx.suites[i].name);
    if (len > maxlen)
      maxlen = len;
  }

  size_t colwidth = maxlen + 2; /* padding between columns */
  size_t ncols = TTY_WIDTH / colwidth;
  if (ncols == 0)
    ncols = 1; /* name wider than terminal, one per line */

  fprintf(stdout, "available test suites (%zu):\n", utest_ctx.nsuites);

  for (size_t i = 0; i < utest_ctx.nsuites; i++) {
    if (i % ncols == 0 && i > 0)
      fputc('\n', stdout);
    fprintf(stdout, "  %-*s", (int)colwidth, utest_ctx.suites[i].name);
  }
  fputc('\n', stdout);
}

void ut_runsuites(void)
{
  for (size_t i = 0; i < utest_ctx.nsuites; i++)
    runsuite(&utest_ctx.suites[i]);
}

void ut_runsuites_th(int nthreads)
{
  if (nthreads <= 0)
    error("invalid arguments");

  atomic_store(&utest_ctx.stop, false);
  atomic_store(&utest_ctx.nextsidx, 0);

  pthread_t *tpool = calloc(nthreads, sizeof(pthread_t));
  if (!tpool)
    fatal();

  for (int i = 0; i < nthreads; i++)
    if (pthread_create(&tpool[i], NULL, runsuite_thread, NULL))
      fatal();

  for (int i = 0; i < nthreads; i++)
    if (pthread_join(tpool[i], NULL))
      fatal();

  free(tpool);
}
