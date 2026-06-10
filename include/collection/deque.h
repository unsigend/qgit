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

#ifndef COL_DEQUE_H
#define COL_DEQUE_H

/* Double-ended queue based on ring buffer implementation. */

#include <stddef.h>

struct deque {
  char *buf;
  size_t elesz;
  size_t sz;
  size_t cap;
  size_t head;
  void (*destroy)(void *);
};

#define deq_empty(deq) ((deq)->sz == 0) /* Check if the deque is empty */
#define deq_size(deq) ((deq)->sz)       /* Get the size of the deque */
#define deq_capacity(deq) ((deq)->cap)  /* Get the capacity of the deque */
#define deq_front(deq)                                                         \
  deq_at((deq), 0) /* Get the front element of the deque                       \
                    */
#define deq_back(deq)                                                          \
  deq_at((deq), deq_size((deq)) - 1) /* Get the back element of the deque */

int deq_init(struct deque *deq, size_t elesz, void (*destroy)(void *));
void deq_fini(struct deque *deq);

void *deq_at(const struct deque *deq, size_t idx);
int deq_resize(struct deque *deq, size_t newsize);
int deq_shrink(struct deque *deq);

int deq_insert(struct deque *deq, size_t idx, void *ele);
int deq_remove(struct deque *deq, size_t idx, void *dest);

int deq_pushback(struct deque *deq, void *ele);
int deq_pushfront(struct deque *deq, void *ele);
int deq_popback(struct deque *deq, void *dest);
int deq_popfront(struct deque *deq, void *dest);

void deq_clear(struct deque *deq);

struct deque_iter {
  struct deque *deq;
  size_t idx;
};

int deq_iter_init(struct deque_iter *iter, struct deque *deq);
void deq_iter_inc(struct deque_iter *iter);
void deq_iter_dec(struct deque_iter *iter);
void *deq_iter_get(struct deque_iter *iter);

#endif
