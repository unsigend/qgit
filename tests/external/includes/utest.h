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

#ifndef UTEST_H
#define UTEST_H

/* Utest is a simple macro-based multi-threaded unit testing framework. This is
   the only public API header needed. */

#define UTEST_INIT(flags)
#define UTEST_FINI()

#define UTEST_ADDFLAG(flag)
#define UTEST_CLRFLAG(flag)

#define UTEST_CASE(name)
#define UTEST_SUITE(name)

#define UTEST_RUNCASE(name)  /* run a test case */
#define UTEST_ADDSUITE(name) /* add a test suite */
#define UTEST_RUNSUITE(                                                        \
    name) /* run a test suite. Return 0 if successful, -1 if not found */
#define UTEST_RUNSUITES() /* run all test suites */
#define UTEST_RUNSUITES_THREAD(                                                \
    nthreads)              /* run all test suites parallel with nthreads */
#define UTEST_SHOWSUITES() /* show all test suites */

/* boolean */
#define EXPECT_TRUE(expr)
#define EXPECT_FALSE(expr)

/* pointer */
#define EXPECT_NULL(ptr)
#define EXPECT_NOTNULL(ptr)
#define EXPECT_EQ_PTR(actual, expect)
#define EXPECT_NE_PTR(actual, expect)
#define EXPECT_GT_PTR(actual, expect)
#define EXPECT_GE_PTR(actual, expect)
#define EXPECT_LT_PTR(actual, expect)
#define EXPECT_LE_PTR(actual, expect)

/* integer */
#define EXPECT_EQ_INT(actual, expect)
#define EXPECT_NE_INT(actual, expect)
#define EXPECT_GT_INT(actual, expect)
#define EXPECT_GE_INT(actual, expect)
#define EXPECT_LT_INT(actual, expect)
#define EXPECT_LE_INT(actual, expect)

/* unsigned integer */
#define EXPECT_EQ_UINT(actual, expect)
#define EXPECT_NE_UINT(actual, expect)
#define EXPECT_GT_UINT(actual, expect)
#define EXPECT_GE_UINT(actual, expect)
#define EXPECT_LT_UINT(actual, expect)
#define EXPECT_LE_UINT(actual, expect)

/* character */
#define EXPECT_EQ_CHAR(actual, expect)
#define EXPECT_NE_CHAR(actual, expect)
#define EXPECT_GT_CHAR(actual, expect)
#define EXPECT_GE_CHAR(actual, expect)
#define EXPECT_LT_CHAR(actual, expect)
#define EXPECT_LE_CHAR(actual, expect)

/* unsigned character */
#define EXPECT_EQ_UCHAR(actual, expect)
#define EXPECT_NE_UCHAR(actual, expect)
#define EXPECT_GT_UCHAR(actual, expect)
#define EXPECT_GE_UCHAR(actual, expect)
#define EXPECT_LT_UCHAR(actual, expect)
#define EXPECT_LE_UCHAR(actual, expect)

/* double */
#define EXPECT_EQ_DOUBLE(actual, expect)
#define EXPECT_NE_DOUBLE(actual, expect)
#define EXPECT_GT_DOUBLE(actual, expect)
#define EXPECT_GE_DOUBLE(actual, expect)
#define EXPECT_LT_DOUBLE(actual, expect)
#define EXPECT_LE_DOUBLE(actual, expect)

/* string */
#define EXPECT_EQ_STR(actual, expect)
#define EXPECT_NE_STR(actual, expect)
#define EXPECT_GT_STR(actual, expect)
#define EXPECT_GE_STR(actual, expect)
#define EXPECT_LT_STR(actual, expect)
#define EXPECT_LE_STR(actual, expect)

#include <utest/core.h>

#endif