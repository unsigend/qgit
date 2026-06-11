/* collection - A generic data structure and algorithms library
 * Copyright (C) 2025 Yixiang Qiu
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "collection/deque.h"

#define MINCAP 16
#define GROWFACTOR 2

#define PINDEX(deq, idx)                                                       \
  (((deq)->head + (idx)) % (deq)->cap) /* Get the physical index */
#define GET(buf, idx, elesz)                                                   \
  ((void *)((char *)(buf) + (idx) * (elesz))) /* Get the element  */

#define overflowcheck(sz, n)                                                   \
  do {                                                                         \
    if ((n) > SIZE_MAX / (sz)) {                                               \
      errno = ERANGE;                                                          \
      return -1;                                                               \
    }                                                                          \
  } while (0) /* Check if the size is overflow */

/* Shift left n elements from [dest+1, dest+n] to [dest, dest+n-1], dest is the
   logical index */
static inline void shiftleft(struct deque *deq, size_t dest, size_t n);

/* Shift right n elements from [dest-n, dest-1] to [dest-n+1, dest], dest is the
   logical index */
static inline void shiftright(struct deque *deq, size_t dest, size_t n);

/* Flatten the deque to the buffer */
static void flatten(struct deque *deq, void *buf);

/* Destroy the elements in the range [start, end) */
static void destroy_r(struct deque *deq, size_t start, size_t end);

static int expand(struct deque *deq);

int deq_init(struct deque *deq, size_t elesz, void (*destroy)(void *))
{
  if (!deq || !elesz)
    return -1;
  memset(deq, 0, sizeof(struct deque));
  deq->elesz = elesz;
  deq->destroy = destroy;
  return 0;
}

void deq_fini(struct deque *deq)
{
  if (!deq)
    return;
  deq_clear(deq);
  if (deq->buf)
    free(deq->buf);
}

void *deq_at(const struct deque *deq, size_t idx)
{
  if (!deq || idx >= deq->sz)
    return NULL;
  return GET(deq->buf, PINDEX(deq, idx), deq->elesz);
}

int deq_resize(struct deque *deq, size_t newsize)
{
  if (!deq)
    return -1;
  if (newsize == deq->sz)
    return 0;
  if (newsize > deq->sz) {
    if (newsize <= deq->cap) {
      deq->sz = newsize;
      return 0;
    } else {
      overflowcheck(deq->elesz, newsize);
      void *newbuf = malloc(newsize * deq->elesz);
      if (!newbuf)
        return -1;
      flatten(deq, newbuf);
      memset((char *)newbuf + deq->sz * deq->elesz, 0,
             (newsize - deq->sz) * deq->elesz);
      free(deq->buf);
      deq->buf = newbuf;
      deq->cap = newsize;
      deq->sz = newsize;
      deq->head = 0;
      return 0;
    }
  } else {
    destroy_r(deq, newsize, deq->sz);
    deq->sz = newsize;
    return 0;
  }
}

int deq_shrink(struct deque *deq)
{
  if (!deq)
    return -1;
  if (deq->sz == deq->cap || !deq->sz)
    return 0;
  void *newbuf = malloc(deq->sz * deq->elesz);
  if (!newbuf)
    return -1;
  flatten(deq, newbuf);
  free(deq->buf);
  deq->buf = newbuf;
  deq->cap = deq->sz;
  deq->head = 0;
  return 0;
}

int deq_insert(struct deque *deq, size_t idx, void *ele)
{
  if (!deq || !ele)
    return -1;
  if (idx >= deq->sz)
    return deq_pushback(deq, ele);
  if (idx == 0)
    return deq_pushfront(deq, ele);
  if (deq->sz == deq->cap && expand(deq) == -1)
    return -1;

  size_t pidx = PINDEX(deq, idx);
  size_t phead = deq->head;
  size_t pend = PINDEX(deq, deq->sz);
  size_t ln, rn;

  if (phead < pend) /* physical continuous */
  {
    ln = pidx - phead;
    rn = pend - pidx;
    if (rn < ln || phead == 0) {
      memmove(GET(deq->buf, pidx + 1, deq->elesz),
              GET(deq->buf, pidx, deq->elesz), rn * deq->elesz);
    } else {
      memmove(GET(deq->buf, phead - 1, deq->elesz),
              GET(deq->buf, phead, deq->elesz), ln * deq->elesz);
      deq->head--;
    }
  } else /* logical continuous */
  {
    if (pidx > phead) {
      ln = pidx - phead;
      rn = (deq->cap - pidx - 1) + pend;
      if (rn < ln) {
        shiftright(deq, idx + rn, rn);
      } else {
        deq->head--;
        memmove(GET(deq->buf, deq->head, deq->elesz),
                GET(deq->buf, deq->head + 1, deq->elesz), ln * deq->elesz);
      }
    } else {
      ln = (deq->cap - phead) + pidx;
      rn = pend - pidx;
      if (rn < ln) {
        memmove(GET(deq->buf, pidx + 1, deq->elesz),
                GET(deq->buf, pidx, deq->elesz), rn * deq->elesz);
      } else {
        deq->head--;
        shiftleft(deq, 0, idx);
      }
    }
  }
  deq->sz++;
  pidx = PINDEX(deq, idx);
  memcpy(GET(deq->buf, pidx, deq->elesz), ele, deq->elesz);
  return 0;
}

int deq_remove(struct deque *deq, size_t idx, void *dest)
{
  if (!deq || idx >= deq->sz)
    return -1;
  if (idx == 0)
    return deq_popfront(deq, dest);
  if (idx == deq->sz - 1)
    return deq_popback(deq, dest);

  size_t pidx = PINDEX(deq, idx);
  size_t phead = deq->head;
  size_t pend = PINDEX(deq, deq->sz);
  size_t ln, rn;

  if (dest)
    memcpy(dest, GET(deq->buf, pidx, deq->elesz), deq->elesz);
  else if (deq->destroy)
    deq->destroy(GET(deq->buf, pidx, deq->elesz));

  if (phead < pend) /* physical continuous */
  {
    ln = pidx - phead;
    rn = pend - pidx - 1;
    if (rn < ln) {
      memmove(GET(deq->buf, pidx, deq->elesz),
              GET(deq->buf, pidx + 1, deq->elesz), rn * deq->elesz);
    } else {
      memmove(GET(deq->buf, phead + 1, deq->elesz),
              GET(deq->buf, phead, deq->elesz), ln * deq->elesz);
      deq->head++;
    }
  } else /* logical continuous */
  {
    if (pidx > phead) {
      ln = pidx - phead;
      rn = (deq->cap - pidx - 1) + pend;
      if (rn < ln) {
        shiftleft(deq, idx, rn);
      } else {
        memmove(GET(deq->buf, phead + 1, deq->elesz),
                GET(deq->buf, phead, deq->elesz), ln * deq->elesz);
        deq->head++;
      }
    } else {
      ln = (deq->cap - phead) + pidx;
      rn = pend - pidx - 1;
      if (rn < ln) {
        memmove(GET(deq->buf, pidx, deq->elesz),
                GET(deq->buf, pidx + 1, deq->elesz), rn * deq->elesz);
      } else {
        shiftright(deq, idx, ln);
        deq->head++;
      }
    }
  }
  deq->sz--;
  return 0;
}

int deq_pushback(struct deque *deq, void *ele)
{
  if (!deq || !ele)
    return -1;
  if (deq->sz == deq->cap && expand(deq) == -1)
    return -1;
  memcpy(GET(deq->buf, PINDEX(deq, deq->sz), deq->elesz), ele, deq->elesz);
  deq->sz++;
  return 0;
}

int deq_pushfront(struct deque *deq, void *ele)
{
  if (!deq || !ele)
    return -1;
  if (deq->sz == deq->cap && expand(deq) == -1)
    return -1;
  size_t newhead = (deq->head + deq->cap - 1) % deq->cap;
  memcpy(GET(deq->buf, newhead, deq->elesz), ele, deq->elesz);
  deq->head = newhead;
  deq->sz++;
  return 0;
}

int deq_popback(struct deque *deq, void *dest)
{
  if (!deq || deq_empty(deq))
    return -1;
  if (dest)
    memcpy(dest, deq_back(deq), deq->elesz);
  else if (deq->destroy)
    deq->destroy(deq_back(deq));
  deq->sz--;
  return 0;
}

int deq_popfront(struct deque *deq, void *dest)
{
  if (!deq || deq_empty(deq))
    return -1;
  if (dest)
    memcpy(dest, deq_front(deq), deq->elesz);
  else if (deq->destroy)
    deq->destroy(deq_front(deq));
  deq->head = (deq->head + 1) % deq->cap;
  deq->sz--;
  return 0;
}

void deq_clear(struct deque *deq)
{
  if (!deq)
    return;
  destroy_r(deq, 0, deq->sz);
  deq->sz = 0;
  deq->head = 0;
}

static int expand(struct deque *deq)
{
  size_t newcap = deq->cap ? deq->cap * GROWFACTOR : MINCAP;
  overflowcheck(deq->elesz, newcap);
  void *newbuf = malloc(newcap * deq->elesz);
  if (!newbuf)
    return -1;
  flatten(deq, newbuf);
  free(deq->buf);
  deq->buf = newbuf;
  deq->cap = newcap;
  deq->head = 0;
  return 0;
}

static void destroy_r(struct deque *deq, size_t start, size_t end)
{
  if (!deq || start >= end || start >= deq->sz)
    return;
  size_t pstart = PINDEX(deq, start);
  size_t pend = PINDEX(deq, end);
  if (deq->destroy) {
    if (pstart < pend) {
      for (size_t i = pstart; i < pend; i++)
        deq->destroy(GET(deq->buf, i, deq->elesz));
    } else {
      for (size_t i = pstart; i < deq->cap; i++)
        deq->destroy(GET(deq->buf, i, deq->elesz));
      for (size_t i = 0; i < pend; i++)
        deq->destroy(GET(deq->buf, i, deq->elesz));
    }
  }
}

static void flatten(struct deque *deq, void *buf)
{
  if (!deq || !buf || deq->sz == 0)
    return;
  size_t phead = deq->head;
  size_t pend = PINDEX(deq, deq->sz);
  if (phead < pend) {
    memcpy(buf, GET(deq->buf, phead, deq->elesz), (pend - phead) * deq->elesz);
  } else {
    memcpy(buf, GET(deq->buf, phead, deq->elesz),
           (deq->cap - phead) * deq->elesz);
    memcpy((char *)buf + (deq->cap - phead) * deq->elesz,
           GET(deq->buf, 0, deq->elesz), (pend - 0) * deq->elesz);
  }
}

static inline void shiftleft(struct deque *deq, size_t dest, size_t n)
{
  for (size_t i = 0; i < n; i++)
    memcpy(GET(deq->buf, PINDEX(deq, dest + i), deq->elesz),
           GET(deq->buf, PINDEX(deq, dest + i + 1), deq->elesz), deq->elesz);
}

static inline void shiftright(struct deque *deq, size_t dest, size_t n)
{
  for (size_t i = 0; i < n; i++)
    memcpy(GET(deq->buf, PINDEX(deq, dest - i), deq->elesz),
           GET(deq->buf, PINDEX(deq, dest - i - 1), deq->elesz), deq->elesz);
}

int deq_iter_init(struct deque_iter *iter, struct deque *deq)
{
  if (!iter || !deq)
    return -1;
  iter->deq = deq;
  iter->idx = 0;
  return 0;
}

void deq_iter_inc(struct deque_iter *iter)
{
  if (!iter)
    return;
  if (iter->idx < iter->deq->sz)
    iter->idx++;
}

void deq_iter_dec(struct deque_iter *iter)
{
  if (!iter)
    return;
  iter->idx--;
}

void *deq_iter_get(struct deque_iter *iter)
{
  if (!iter)
    return NULL;
  return deq_at(iter->deq, iter->idx);
}