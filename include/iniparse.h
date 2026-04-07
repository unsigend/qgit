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

struct iniFILE {
  const char *filename;
  char *buf;
  size_t bufsz;
  size_t nsecs;
  size_t nsecscap;
  struct ini_sec *secs;
};

struct ini_sec {
  const char *sec;
  size_t nentries;
  size_t nentriescap;
  struct ini_entry *entries;
};

struct ini_entry {
  const char *key;
  const char *val;
};

#ifdef __cplusplus
extern "C" {
#endif

extern struct iniFILE *iniparse_open(const char *filename);
extern struct iniFILE *iniparse_create(const char *filename);
extern void iniparse_close(struct iniFILE *fp);
extern int iniparse_parse(struct iniFILE *fp);

/* Return the value of the key in the section, NULL if not found or key or
   section not exists. */
extern const char *iniparse_get(struct iniFILE *fp, const char *sec,
                                const char *key);
extern int iniparse_set(struct iniFILE *fp, const char *sec, const char *key,
                        const char *val);
extern int iniparse_unset(struct iniFILE *fp, const char *sec, const char *key);

/* Print the INI parsed content with the format of [section].[key]=[value]
   to the output stream or a buffer. Return the number of bytes written on
   success, -1 on error. */
extern int iniparse_fprint(struct iniFILE *fp, FILE *stream);
extern int iniparse_sprint(struct iniFILE *fp, char *buf);
extern int iniparse_snprint(struct iniFILE *fp, char *buf, size_t bufsz);

/* Write the file content to the current INI file or a new file. Return 0 on
   success, -1 on error. */
extern int iniparse_write(struct iniFILE *fp);
extern int iniparse_writeto(struct iniFILE *fp, const char *filename);

#ifdef __cplusplus
}
#endif

#endif