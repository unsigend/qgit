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

#ifndef UTEST_FLAGS_H
#define UTEST_FLAGS_H

#define UT_PASS 0
#define UT_FAIL 1

#define UT_CASE 0
#define UT_SUITE 1

#define UTF_STOPONASS                                                          \
  (1 << 0) /* stop on assertion failure in a case, skip the rest of the tests  \
              in a case */
#define UTF_STOPONCASE                                                         \
  (1 << 1) /* stop on case failure, skip the rest of the cases in a suite  */
#define UTF_STOPONSUITE                                                        \
  (1 << 2) /* stop on suite failure, skip the rest of the suites in list       \
            */
#define UTF_SHOWCASE (1 << 10)                       /* show case */
#define UTF_SHOWSUITE (1 << 11)                      /* show suite */
#define UTF_FULLSTYLE (UTF_SHOWCASE | UTF_SHOWSUITE) /* full style */
#define UTF_BRIEFSTYLE (UTF_SHOWSUITE)               /* brief style */
#define UTF_DEFAULT (UTF_STOPONASS | UTF_FULLSTYLE)  /* default flags */

#endif