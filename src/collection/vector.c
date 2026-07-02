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

#include <collection/vector.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define MINCAP 16
#define GROWFACTOR 2

#define GET(vec, buf, idx)                                                     \
    ((void *)((char *)(buf) + (idx) * (vec)->elesz)) /* Get the element  */

#define overflowcheck(sz, n)                                                   \
    do {                                                                       \
        if ((n) > SIZE_MAX / (sz)) {                                           \
            errno = ERANGE;                                                    \
            return -1;                                                         \
        }                                                                      \
    } while (0) /* Check if the size is overflow */

struct vector_iter {
    struct vector *vec;
    size_t idx;
};

static void destroy_range(struct vector *vec, size_t start, size_t end);

int vec_init(struct vector **vec, size_t elesz, void (*destroy)(void *))
{
    if (!vec || !elesz)
        return -1;

    struct vector *v = calloc(1, sizeof(struct vector));
    if (!v)
        return -1;
    v->elesz = elesz;
    v->free = destroy;
    *vec = v;
    return 0;
}

void vec_free(struct vector *vec)
{
    if (!vec)
        return;
    vec_clear(vec);
    if (vec->buf)
        free(vec->buf);
    free(vec);
}

void (*vec_free_func(const struct vector *vec))(void *)
{
    if (!vec)
        return NULL;
    return vec->free;
}

size_t vec_elesz(const struct vector *vec)
{
    if (!vec)
        return 0;
    return vec->elesz;
}

int vec_empty(const struct vector *vec)
{
    if (!vec)
        return 1;
    return vec->sz == 0;
}

void *vec_raw(const struct vector *vec)
{
    if (!vec)
        return NULL;
    return vec->buf;
}

size_t vec_size(const struct vector *vec)
{
    if (!vec)
        return 0;
    return vec->sz;
}

size_t vec_capacity(const struct vector *vec)
{
    if (!vec)
        return 0;
    return vec->cap;
}

void *vec_back(const struct vector *vec)
{
    if (!vec)
        return NULL;
    if (vec_empty(vec))
        return NULL;
    return GET(vec, vec->buf, vec->sz - 1);
}

void *vec_front(const struct vector *vec)
{
    if (!vec)
        return NULL;
    if (vec_empty(vec))
        return NULL;
    return GET(vec, vec->buf, 0);
}

void *vec_at(const struct vector *vec, size_t idx)
{
    if (!vec || idx >= vec->sz)
        return NULL;
    return GET(vec, vec->buf, idx);
}

int vec_resize(struct vector *vec, size_t newsz)
{
    if (!vec)
        return -1;
    if (newsz == vec->sz)
        return 0;
    if (newsz > vec->sz) {
        if (newsz <= vec->cap) {
            vec->sz = newsz;
            return 0;
        } else {
            overflowcheck(vec->elesz, newsz);
            void *newbuf = realloc(vec->buf, newsz * vec->elesz);
            if (!newbuf)
                return -1;
            vec->buf = newbuf;
            vec->cap = newsz;
            vec->sz = newsz;
            return 0;
        }
    } else {
        destroy_range(vec, newsz, vec->sz);
        vec->sz = newsz;
        return 0;
    }
}

int vec_shrink(struct vector *vec)
{
    if (!vec)
        return -1;
    if (vec->sz == vec->cap || !vec->sz)
        return 0;
    overflowcheck(vec->elesz, vec->sz);
    void *newbuf = realloc(vec->buf, vec->sz * vec->elesz);
    if (!newbuf)
        return -1;
    vec->buf = newbuf;
    vec->cap = vec->sz;
    return 0;
}

int vec_pushback(struct vector *vec, void *ele)
{
    if (!vec || !ele)
        return -1;
    if (vec->sz == vec->cap) {
        size_t newcap = vec->cap ? vec->cap * GROWFACTOR : MINCAP;
        overflowcheck(vec->elesz, newcap);
        void *newbuf = realloc(vec->buf, newcap * vec->elesz);
        if (!newbuf)
            return -1;
        vec->buf = newbuf;
        vec->cap = newcap;
    }
    memcpy(GET(vec, vec->buf, vec->sz), ele, vec->elesz);
    vec->sz++;
    return 0;
}

int vec_popback(struct vector *vec, void *dest)
{
    if (!vec || vec_empty(vec))
        return -1;

    if (dest)
        memcpy(dest, vec_back(vec), vec->elesz);
    else if (vec->free)
        vec->free(vec_back(vec));

    vec->sz--;
    return 0;
}

int vec_insert(struct vector *vec, size_t idx, void *ele)
{
    if (!vec || !ele)
        return -1;
    if (idx >= vec->sz)
        return vec_pushback(vec, ele);

    if (vec->sz == vec->cap) {
        size_t newcap = vec->cap ? vec->cap * GROWFACTOR : MINCAP;
        overflowcheck(vec->elesz, newcap);
        void *newbuf = malloc(newcap * vec->elesz);
        if (!newbuf)
            return -1;
        memcpy(newbuf, vec->buf, vec->elesz * idx);
        memcpy((char *)newbuf + vec->elesz * idx, ele, vec->elesz);
        memcpy((char *)newbuf + vec->elesz * (idx + 1), GET(vec, vec->buf, idx),
               vec->elesz * (vec->sz - idx));
        free(vec->buf);
        vec->buf = newbuf;
        vec->cap = newcap;
        vec->sz++;
        return 0;
    }
    memmove(GET(vec, vec->buf, idx + 1), GET(vec, vec->buf, idx),
            vec->elesz * (vec->sz - idx));
    memcpy(GET(vec, vec->buf, idx), ele, vec->elesz);
    vec->sz++;
    return 0;
}

int vec_remove(struct vector *vec, size_t idx, void *dest)
{
    if (!vec || idx >= vec->sz)
        return -1;
    if (dest)
        memcpy(dest, GET(vec, vec->buf, idx), vec->elesz);
    else if (vec->free)
        vec->free(GET(vec, vec->buf, idx));
    memmove(GET(vec, vec->buf, idx), GET(vec, vec->buf, idx + 1),
            vec->elesz * (vec->sz - idx - 1));
    vec->sz--;
    return 0;
}

void vec_sort(struct vector *vec, int (*cmp)(const void *, const void *))
{
    if (!vec || !cmp)
        return;
    if (!vec->buf)
        return;
    qsort(vec->buf, vec->sz, vec->elesz, cmp);
}

void vec_clear(struct vector *vec)
{
    if (!vec)
        return;
    destroy_range(vec, 0, vec->sz);
    vec->sz = 0;
}

int vec_iter_init(struct vector_iter **iter, struct vector *vec)
{
    if (!iter || !vec)
        return -1;
    struct vector_iter *i = calloc(1, sizeof(struct vector_iter));
    if (!i)
        return -1;
    i->vec = vec;
    i->idx = 0;
    *iter = i;
    return 0;
}

void vec_iter_inc(struct vector_iter *iter)
{
    if (!iter)
        return;
    if (iter->idx < iter->vec->sz)
        iter->idx++;
}

void vec_iter_dec(struct vector_iter *iter)
{
    if (!iter)
        return;
    iter->idx--;
}

void *vec_iter_get(struct vector_iter *iter)
{
    if (!iter)
        return NULL;
    return vec_at(iter->vec, iter->idx);
}

void vec_iter_free(struct vector_iter *iter)
{
    if (!iter)
        return;
    free(iter);
}

static void destroy_range(struct vector *vec, size_t start, size_t end)
{
    if (!vec || start >= end || start >= vec->sz)
        return;
    if (vec->free) {
        for (size_t i = start; i < end; i++)
            vec->free(GET(vec, vec->buf, i));
    }
}
