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

#ifndef INIPARSE_H
#define INIPARSE_H

/* INI file parser library. */

#include <stddef.h>
#include <stdio.h>

struct iniparse_ctx {
  const char *filename;
  char *buf;
  size_t bufsz;
  size_t nentries;
  size_t nentriescap;
  struct iniparse_entry *entries;
};

struct iniparse_entry {
  const char *sec;
  const char *key;
  const char *val;
};

#ifdef __cplusplus
extern "C" {
#endif

/* Return 0 on success, -1 on error. */
extern int iniparse_init(struct iniparse_ctx *ctx, const char *filename);
extern int iniparse_parse(struct iniparse_ctx *ctx);
extern void iniparse_fini(struct iniparse_ctx *ctx);

/* Return the value of the key in the section, NULL if not found or key, section
   not exists. */
extern const char *iniparse_getvalue(struct iniparse_ctx *ctx, const char *sec,
                                     const char *key);

#ifdef __cplusplus
}
#endif

#endif