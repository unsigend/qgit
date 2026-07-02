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

#include "vector_struct.h"

#include <collection/heap.h>
#include <collection/util.h>
#include <collection/vector.h>
#include <stdlib.h>
#include <string.h>

#define PARENT(idx) (((idx) - 1) / 2)
#define LEFT(idx) (2 * (idx) + 1)
#define RIGHT(idx) (2 * (idx) + 2)

struct heap {
    struct vector *vec;
    int (*cmp)(void *, void *);
};

static int shiftup(struct heap *heap, size_t idx);
static int shiftdown(struct heap *heap, size_t idx);

int heap_init(struct heap **heap, size_t elesz, int (*cmp)(void *, void *),
              void (*destroy)(void *))
{
    if (!heap || !elesz || !cmp)
        return -1;
    struct heap *h = calloc(1, sizeof(struct heap));
    if (!h)
        return -1;

    int ret = vec_init(&h->vec, elesz, destroy);
    if (ret == -1) {
        free(h);
        return -1;
    }

    h->cmp = cmp;
    *heap = h;
    return 0;
}

void heap_free(struct heap *heap)
{
    if (!heap)
        return;
    vec_free(heap->vec);
    free(heap);
}

int heap_empty(const struct heap *heap)
{
    if (!heap)
        return 1;
    return vec_empty(heap->vec);
}

size_t heap_size(const struct heap *heap)
{
    if (!heap)
        return 0;
    return vec_size(heap->vec);
}

void *heap_peek(const struct heap *heap)
{
    if (!heap)
        return NULL;
    return vec_front(heap->vec);
}

int heap_push(struct heap *heap, void *ele)
{
    if (!heap || !ele)
        return -1;
    if (vec_pushback(heap->vec, ele) == -1)
        return -1;
    if (vec_size(heap->vec) == 1)
        return 0;
    return shiftup(heap, vec_size(heap->vec) - 1);
}

int heap_pop(struct heap *heap, void *dest)
{
    if (!heap || vec_empty(heap->vec))
        return -1;

    if (dest)
        memcpy(dest, vec_raw(heap->vec), vec_elesz(heap->vec));
    else if (heap->vec->free)
        heap->vec->free(vec_raw(heap->vec));

    /* Move semantic only no destroy for tail element */
    if (vec_size(heap->vec) > 1)
        memcpy(vec_raw(heap->vec), vec_at(heap->vec, vec_size(heap->vec) - 1),
               vec_elesz(heap->vec));

    /* Must use direct member access to modify the size */
    heap->vec->sz--;

    if (vec_empty(heap->vec))
        return 0;
    return shiftdown(heap, 0);
}

void heap_clear(struct heap *heap)
{
    if (!heap)
        return;
    vec_clear(heap->vec);
}

static int shiftup(struct heap *heap, size_t idx)
{
    if (!heap || idx >= vec_size(heap->vec))
        return -1;
    size_t p = PARENT(idx);
    while (idx && heap->cmp(vec_at(heap->vec, idx), vec_at(heap->vec, p)) < 0) {
        swap(vec_at(heap->vec, idx), vec_at(heap->vec, p),
             vec_elesz(heap->vec));
        idx = p;
        p = PARENT(idx);
    }
    return 0;
}

static int shiftdown(struct heap *heap, size_t idx)
{
    if (!heap)
        return -1;
    while (1) {
        size_t i = idx;
        size_t l = LEFT(idx);
        size_t r = RIGHT(idx);

        if (l < vec_size(heap->vec) &&
            heap->cmp(vec_at(heap->vec, l), vec_at(heap->vec, i)) < 0)
            i = l;
        if (r < vec_size(heap->vec) &&
            heap->cmp(vec_at(heap->vec, r), vec_at(heap->vec, i)) < 0)
            i = r;
        if (i == idx)
            break;
        swap(vec_at(heap->vec, idx), vec_at(heap->vec, i),
             vec_elesz(heap->vec));
        idx = i;
    }
    return 0;
}
