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

#include <ctype.h>
#include <iniparse.h>
#include <stdlib.h>
#include <string.h>

#define NLIST 64
#define FACTOR 2

static inline char *skipline(char *p)
{
  while (*p && *p != '\n')
    p++;
  if (*p == '\n')
    p++;
  return p;
}

static inline char *skipspace(char *p)
{
  while (*p && *p != '\n' && isspace(*p))
    p++;
  return p;
}

static int putentry(struct ini_sec *sec, char *key, char *val)
{
  if (sec->nentries >= sec->nentriescap) {
    if (sec->entries) {
      sec->nentriescap *= FACTOR;
      struct ini_entry *newentries =
          realloc(sec->entries, sec->nentriescap * sizeof(*sec->entries));
      if (!newentries)
        return -1;
      sec->entries = newentries;
    } else {
      sec->nentriescap = NLIST;
      sec->entries = calloc(sec->nentriescap, sizeof(*sec->entries));
      if (!sec->entries)
        return -1;
    }
  }
  sec->entries[sec->nentries].key = strdup(key);
  if (!sec->entries[sec->nentries].key)
    return -1;
  if (val) {
    sec->entries[sec->nentries].val = strdup(val);
    if (!sec->entries[sec->nentries].val) {
      free((void *)sec->entries[sec->nentries].key);
      return -1;
    }
  } else
    sec->entries[sec->nentries].val = NULL;

  sec->nentries++;
  return 0;
}

static int put(struct iniFILE *fp, char *sec, char *key, char *val)
{
  if (!sec)
    return -1;
  for (size_t i = 0; i < fp->nsecs; i++) {
    if (!strcmp(fp->secs[i].sec, sec)) {
      return putentry(&fp->secs[i], key, val);
    }
  }
  if (fp->nsecs >= fp->nsecscap) {
    if (fp->secs) {
      fp->nsecscap *= FACTOR;
      struct ini_sec *newsecs =
          realloc(fp->secs, fp->nsecscap * sizeof(*fp->secs));
      if (!newsecs)
        return -1;
      fp->secs = newsecs;
    } else {
      fp->nsecscap = NLIST;
      fp->secs = calloc(fp->nsecscap, sizeof(*fp->secs));
      if (!fp->secs)
        return -1;
    }
  }
  fp->secs[fp->nsecs].sec = strdup(sec);
  if (!fp->secs[fp->nsecs].sec)
    return -1;
  fp->secs[fp->nsecs].entries = NULL;
  fp->secs[fp->nsecs].nentries = 0;
  fp->secs[fp->nsecs].nentriescap = 0;
  fp->nsecs++;
  return putentry(&fp->secs[fp->nsecs - 1], key, val);
}

static char *parse_section(char *p, char **sec)
{
  ++p; /* skip [ */
  p = skipspace(p);
  *sec = p;

  while (*p && *p != ']' && *p != '\n')
    p++;
  if (*p != ']') {
    *sec = NULL;
    return p + 1;
  }
  char *end = p;

  while (p > *sec && isspace(*(p - 1)))
    --p;
  *p = '\0';

  return skipline(end + 1);
}

static char *parse_entry(char *p, char **key, char **val)
{
  char *next, *s;
  *key = p;
  while (*p && *p != '=' && *p != '\n')
    p++;

  s = p;
  if (*p == '=') {
    p = skipspace(p + 1);
    *val = p;
    while (*p && *p != '\n')
      p++;
    char *r = p;
    while (r > *val && isspace(*(r - 1)))
      --r;
    *r = '\0';
    next = p + 1;
  } else {
    *val = NULL;
    next = skipline(p + 1);
  }
  while (s > *key && isspace(*(s - 1)))
    --s;
  *s = '\0';
  return next;
}

static struct ini_entry *getentry(struct iniFILE *fp, const char *sec,
                                  const char *key)
{
  for (size_t i = 0; i < fp->nsecs; i++) {
    if (!strcmp(fp->secs[i].sec, sec)) {
      for (size_t j = 0; j < fp->secs[i].nentries; j++) {
        struct ini_entry *entry = &fp->secs[i].entries[j];
        if (entry->key && !strcmp(entry->key, key))
          return entry;
      }
      break;
    }
  }
  return NULL;
}

int iniparse_parse(struct iniFILE *fp)
{
  if (!fp)
    return -1;
  if (!fp->buf)
    return 0;
  char *p = fp->buf;
  char *sec = NULL, *key = NULL, *val = NULL;

  while (p < fp->buf + fp->bufsz) {
    p = skipspace(p);
    switch (*p) {
    case '\0':
      return 0;
    case '\n': /* blank line or comment */
    case ';':
    case '#':
      p = skipline(p);
      break;
    case '[': /* section */
      p = parse_section(p, &sec);
      break;
    default: /* entry */
      p = parse_entry(p, &key, &val);
      if (put(fp, sec, key, val) == -1)
        return -1;
      break;
    }
  }
  return 0;
}

const char *iniparse_get(struct iniFILE *fp, const char *sec, const char *key)
{
  struct ini_entry *entry = getentry(fp, sec, key);
  return entry ? entry->val : NULL;
}

int iniparse_set(struct iniFILE *fp, const char *sec, const char *key,
                 const char *val)
{
  struct ini_entry *entry = getentry(fp, sec, key);
  if (entry) {
    free((void *)entry->val);
    entry->val = val ? strdup(val) : NULL;
    if (val && !entry->val)
      return -1;
    return 0;
  } else
    return put(fp, (char *)sec, (char *)key, (char *)val);
}

int iniparse_unset(struct iniFILE *fp, const char *sec, const char *key)
{
  /* lazy delete */
  struct ini_entry *entry = getentry(fp, sec, key);
  if (entry) {
    if (entry->val)
      free((void *)entry->val);
    if (entry->key)
      free((void *)entry->key);
    entry->val = NULL;
    entry->key = NULL;
    return 0;
  }
  return -1;
}