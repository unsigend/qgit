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

#ifndef COL_HEAP_H
#define COL_HEAP_H

#include "collection/vector.h"

#include <stddef.h>

struct heap;

#define heap_empty(heap)                                                       \
  vec_empty(&(heap)->vec)                      /* Check if the heap is empty */
#define heap_size(heap) vec_size(&(heap)->vec) /* Get the size of the heap */

int heap_init(struct heap *heap, size_t elesz, int (*cmp)(void *, void *),
              void (*destroy)(void *));
void heap_fini(struct heap *heap);

int heap_push(struct heap *heap, void *ele);
int heap_pop(struct heap *heap, void *dest);
void *heap_peek(struct heap *heap);
void heap_clear(struct heap *heap);

struct heap {
  struct vector vec;
  int (*cmp)(void *, void *);
};

#endif