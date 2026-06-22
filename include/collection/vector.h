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

#ifndef COL_VECTOR_H
#define COL_VECTOR_H

/* Vector is designed based on flat slab raw buffer. It is optimized for cache
   locality. */
#include <stddef.h>

struct vector {
  char *buf;
  size_t elesz;
  size_t sz;
  size_t cap;
  void (*destroy)(void *);
};

#define vec_empty(vec) ((vec)->sz == 0) /* Check if the vector is empty */
#define vec_raw(vec) ((vec)->buf)       /* Get the raw buffer of the vector */
#define vec_size(vec) ((vec)->sz)       /* Get the size of the vector */
#define vec_capacity(vec) ((vec)->cap)  /* Get the capacity of the vector */
#define vec_back(vec)                                                          \
  vec_at((vec), vec_size((vec)) - 1) /* Get the last element of the vector */
#define vec_front(vec)                                                         \
  vec_at((vec), 0) /* Get the first element of the vector */

int vec_init(struct vector *vec, size_t elesz, void (*destroy)(void *));
void vec_fini(struct vector *vec);

void *vec_at(const struct vector *vec, size_t idx);
int vec_resize(struct vector *vec, size_t newsz);
int vec_shrink(struct vector *vec);

int vec_pushback(struct vector *vec, void *ele);
int vec_popback(struct vector *vec, void *dest);
int vec_insert(struct vector *vec, size_t idx, void *ele);
int vec_remove(struct vector *vec, size_t idx, void *dest);
void vec_sort(struct vector *vec, int (*cmp)(const void *, const void *));
void vec_clear(struct vector *vec);

struct vector_iter {
  struct vector *vec;
  size_t idx;
};

int vec_iter_init(struct vector_iter *iter, struct vector *vec);
void vec_iter_inc(struct vector_iter *iter);
void vec_iter_dec(struct vector_iter *iter);
void *vec_iter_get(struct vector_iter *iter);

#endif