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

#ifndef UTEST_TYPES_H
#define UTEST_TYPES_H

#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>
#include <utest/print.h>

struct utsuite;
struct utcase;

typedef void (*utcase_func)(struct utcase *);
typedef void (*utsuite_func)(struct utsuite *);

struct utsuite {
  const char *name;  /* suite name */
  utsuite_func func; /* function ptr */
  size_t cntotal;    /* total cases */
  size_t cnpassed;   /* passed cases */
  size_t cnfailed;   /* failed cases */
  size_t cnskipped;  /* skipped cases */
  struct utbuf buf;  /* buffer for suite output */
};

struct utcase {
  const char *name;  /* case name */
  utcase_func func;  /* function ptr */
  int status;        /* case status */
  struct utbuf *buf; /* buffer for case output */
};

struct ut_ctx {
  int flags;              /* context flags */
  atomic_size_t nextsidx; /* next suite index to dispatch */
  atomic_bool stop;       /* stop suite flag */
  size_t snpassed;        /* passed suites */
  size_t snfailed;        /* failed suites */
  size_t snskipped;       /* skipped suites */
  size_t cnpassed;        /* passed cases */
  size_t cnfailed;        /* failed cases */
  size_t cnskipped;       /* skipped cases */
  struct utsuite *suites; /* suites list */
  size_t nsuites;         /* number of suites */
  size_t nsuitescap;      /* capacity of suites list */
  pthread_mutex_t lock;   /* lock for the test context */
  struct timespec tstart; /* start time */
  struct timespec tend;   /* end time */
};

#endif