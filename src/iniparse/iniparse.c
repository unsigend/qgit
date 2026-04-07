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
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

static inline void freeentry(struct ini_entry *entry)
{
  if (!entry)
    return;
  if (entry->key)
    free((void *)entry->key);
  if (entry->val)
    free((void *)entry->val);
}

static inline void freesec(struct ini_sec *sec)
{
  if (!sec)
    return;
  if (sec->sec)
    free((void *)sec->sec);
  if (sec->entries) {
    for (size_t i = 0; i < sec->nentries; i++)
      freeentry(&sec->entries[i]);
    free(sec->entries);
  }
}

struct iniFILE *iniparse_open(const char *filename)
{
  int fd;
  off_t sz;
  if (!filename)
    return NULL;

  struct iniFILE *fp = malloc(sizeof(*fp));
  if (!fp)
    return NULL;
  memset(fp, 0, sizeof(*fp));
  fp->filename = strdup(filename);
  if (!fp->filename) {
    free(fp);
    return NULL;
  }

  if ((fd = open(filename, O_RDONLY)) == -1) {
    free(fp);
    return NULL;
  }

  sz = lseek(fd, 0, SEEK_END);
  if (sz == -1 || !sz) {
    close(fd);
    free(fp);
    return NULL;
  }

  char *buf =
      (char *)mmap(NULL, sz + 1, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
  close(fd);
  if (buf == MAP_FAILED) {
    free(fp);
    return NULL;
  }
  buf[sz] = '\0'; /* EOF guard */

  fp->buf = buf;
  fp->bufsz = (size_t)sz;
  return fp;
}

struct iniFILE *iniparse_create(const char *filename)
{
  if (!filename)
    return NULL;

  struct iniFILE *fp = malloc(sizeof(*fp));
  if (!fp)
    return NULL;
  memset(fp, 0, sizeof(*fp));
  fp->filename = strdup(filename);
  if (!fp->filename) {
    free(fp);
    return NULL;
  }

  return fp;
}

void iniparse_close(struct iniFILE *fp)
{
  if (!fp)
    return;

  if (fp->buf)
    munmap((void *)fp->buf, fp->bufsz + 1);

  if (fp->secs) {
    for (size_t i = 0; i < fp->nsecs; i++)
      freesec(&fp->secs[i]);
    free(fp->secs);
  }
  if (fp->filename)
    free((void *)fp->filename);
  free(fp);
}
