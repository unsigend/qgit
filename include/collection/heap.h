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

#ifndef COLLECTION_HEAP_H
#define COLLECTION_HEAP_H

#include <stddef.h>

struct heap;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Allocate and initialize an empty heap. Each stored element is a copy of the
 * value passed to push operations.
 *
 * @param heap    output pointer to receive the new heap, must not be NULL
 * @param elesz   byte size of each element, must be non zero
 * @param cmp     comparator used to order elements, must not be NULL
 * @param destroy optional callback invoked on each element when it is
 *                discarded, or NULL for no op
 * @return        0 on success, -1 on failure
 */
extern int heap_init(struct heap **heap, size_t elesz,
                     int (*cmp)(void *, void *), void (*destroy)(void *));

/**
 * Destroy all elements, release the internal buffer, and free the heap.
 *
 * @param heap the heap to free, or NULL
 */
extern void heap_free(struct heap *heap);

/**
 * Test whether the heap contains no elements.
 *
 * @param heap the heap to inspect, or NULL
 * @return     non zero if empty, zero otherwise
 */
extern int heap_empty(const struct heap *heap);

/**
 * Return the current number of elements in the heap.
 *
 * @param heap the heap to inspect, or NULL
 * @return     element count, or 0 if heap is NULL
 */
extern size_t heap_size(const struct heap *heap);

/**
 * Return a pointer to the root element without removing it.
 *
 * @param heap the heap to inspect, or NULL
 * @return     pointer to the root element, or NULL if heap is NULL or empty
 */
extern void *heap_peek(const struct heap *heap);

/**
 * Push a copy of ele into the heap and restore the heap property.
 *
 * @param heap the heap to modify
 * @param ele  pointer to the value to copy, must not be NULL
 * @return     0 on success, -1 on failure
 */
extern int heap_push(struct heap *heap, void *ele);

/**
 * Remove the root element. When dest is non NULL, copy the element there and
 * skip destroy, otherwise invoke destroy when set.
 *
 * @param heap the heap to modify
 * @param dest buffer of at least elesz bytes to receive the element, or NULL
 *             to invoke destroy
 * @return     0 on success, -1 if heap is NULL or the heap is empty
 */
extern int heap_pop(struct heap *heap, void *dest);

/**
 * Remove all elements and reset size to zero, invoking destroy on each when
 * set. Does not release the buffer or free the heap.
 *
 * @param heap the heap to clear, or NULL
 */
extern void heap_clear(struct heap *heap);

#ifdef __cplusplus
}
#endif

#endif
