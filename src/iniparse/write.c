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

#include <fcntl.h>
#include <iniparse.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

int iniparse_fprint(struct iniFILE *fp, FILE *stream)
{
  int nw = 0;
  for (size_t i = 0; i < fp->nsecs; i++) {
    struct ini_sec *sec = &fp->secs[i];
    for (size_t j = 0; j < sec->nentries; j++) {
      struct ini_entry *entry = &sec->entries[j];
      if (entry->key)
        nw += fprintf(stream, "%s.%s=%s\n", sec->sec, entry->key,
                      entry->val ? entry->val : "");
    }
  }
  return nw;
}

int iniparse_sprint(struct iniFILE *fp, char *buf)
{
  return iniparse_snprint(fp, buf, SIZE_MAX);
}

int iniparse_snprint(struct iniFILE *fp, char *buf, size_t bufsz)
{
  int nw = 0;
  size_t remsz = bufsz;
  for (size_t i = 0; i < fp->nsecs; i++) {
    struct ini_sec *sec = &fp->secs[i];
    for (size_t j = 0; j < sec->nentries; j++) {
      struct ini_entry *entry = &sec->entries[j];
      if (entry->key) {
        int n = snprintf(buf + nw, remsz, "%s.%s=%s\n", sec->sec, entry->key,
                         entry->val ? entry->val : "");
        nw += n;
        remsz = (size_t)nw < bufsz ? bufsz - nw : 0;
      }
    }
  }
  return nw;
}

int iniparse_write(struct iniFILE *fp)
{
  return iniparse_writeto(fp, fp->filename);
}

int iniparse_writeto(struct iniFILE *fp, const char *filename)
{
  char buf[PATH_MAX];
  snprintf(buf, sizeof(buf), "%s.lock", filename);
  FILE *lockfile = fopen(buf, "w");
  if (!lockfile)
    return -1;

  int nw = 0;
  for (size_t i = 0; i < fp->nsecs; i++) {
    struct ini_sec *sec = &fp->secs[i];
    nw += fprintf(lockfile, "[%s]\n", sec->sec);
    for (size_t j = 0; j < sec->nentries; j++) {
      struct ini_entry *entry = &sec->entries[j];
      if (entry->key)
        nw += fprintf(lockfile, "\t%s=%s\n", entry->key,
                      entry->val ? entry->val : "");
    }
  }
  fclose(lockfile);
  if (rename(buf, filename) == -1) {
    unlink(buf);
    return -1;
  }
  return nw;
}