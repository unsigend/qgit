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

#ifndef COLLECTION_VECTOR_H
#define COLLECTION_VECTOR_H

#include <stddef.h>

struct vector;
struct vector_iter;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Allocate and initialize an empty vector. Each stored element is a copy of the
 * value passed to insert or push operations.
 *
 * @param vec     output pointer to receive the new vector, must not be NULL
 * @param elesz   byte size of each element, must be non zero
 * @param destroy optional callback invoked on each element when it is
 *                discarded, or NULL for no op
 * @return        0 on success, -1 on failure
 */
extern int vec_init(struct vector **vec, size_t elesz, void (*destroy)(void *));

/**
 * Destroy all elements, release the internal buffer, and free the vector.
 *
 * @param vec the vector to free, or NULL
 */
extern void vec_free(struct vector *vec);

/**
 * Return the element free callback configured at initialization.
 *
 * @param vec the vector to inspect, or NULL
 * @return    the callback passed to vec_init, or NULL if vec is NULL or no
 *            callback was set
 */
extern void (*vec_free_func(const struct vector *vec))(void *);

/**
 * Return the byte size of each element stored in the vector.
 *
 * @param vec the vector to inspect, or NULL
 * @return    element size in bytes, or 0 if vec is NULL
 */
extern size_t vec_elesz(const struct vector *vec);

/**
 * Test whether the vector contains no elements.
 *
 * @param vec the vector to inspect, or NULL
 * @return    non zero if empty, zero otherwise
 */
extern int vec_empty(const struct vector *vec);

/**
 * Return the raw contiguous buffer holding all elements.
 *
 * @param vec the vector to inspect, or NULL
 * @return    pointer to the element buffer, or NULL if vec is NULL
 */
extern void *vec_raw(const struct vector *vec);

/**
 * Return the current number of elements in the vector.
 *
 * @param vec the vector to inspect, or NULL
 * @return    element count, or 0 if vec is NULL
 */
extern size_t vec_size(const struct vector *vec);

/**
 * Return the number of allocated element slots in the buffer.
 *
 * @param vec the vector to inspect, or NULL
 * @return    capacity in elements, or 0 if vec is NULL
 */
extern size_t vec_capacity(const struct vector *vec);

/**
 * Return a pointer to the last element.
 *
 * @param vec the vector to inspect, or NULL
 * @return    pointer to the last element, or NULL if vec is NULL or empty
 */
extern void *vec_back(const struct vector *vec);

/**
 * Return a pointer to the first element.
 *
 * @param vec the vector to inspect, or NULL
 * @return    pointer to the first element, or NULL if vec is NULL or empty
 */
extern void *vec_front(const struct vector *vec);

/**
 * Return a pointer to the element at the given index.
 *
 * @param vec the vector to inspect, or NULL
 * @param idx zero based element index
 * @return    pointer to the element, or NULL if vec is NULL or idx is out of
 *            range
 */
extern void *vec_at(const struct vector *vec, size_t idx);

/**
 * Set the logical length of the vector. Growing leaves new slots uninitialized.
 * Shrinking invokes destroy on each removed element when set.
 *
 * @param vec   the vector to resize
 * @param newsz new element count
 * @return      0 on success, -1 on failure
 */
extern int vec_resize(struct vector *vec, size_t newsz);

/**
 * Reallocate the buffer so capacity matches the current size, releasing unused
 * slots.
 *
 * @param vec the vector to shrink
 * @return    0 on success, -1 on failure
 */
extern int vec_shrink(struct vector *vec);

/**
 * Append a copy of ele to the end of the vector, growing the buffer if needed.
 *
 * @param vec the vector to modify
 * @param ele pointer to the value to copy, must not be NULL
 * @return    0 on success, -1 on failure
 */
extern int vec_pushback(struct vector *vec, void *ele);

/**
 * Remove the last element. When dest is non NULL, copy the element there and
 * skip destroy, otherwise invoke destroy when set.
 *
 * @param vec  the vector to modify
 * @param dest buffer of at least elesz bytes to receive the element, or NULL to
 *             invoke destroy
 * @return     0 on success, -1 if vec is NULL or the vector is empty
 */
extern int vec_popback(struct vector *vec, void *dest);

/**
 * Insert a copy of ele before idx, shifting subsequent elements right. When
 * idx is at or beyond the current size, behaves like vec_pushback.
 *
 * @param vec the vector to modify
 * @param idx zero based insertion position
 * @param ele pointer to the value to copy, must not be NULL
 * @return    0 on success, -1 on failure
 */
extern int vec_insert(struct vector *vec, size_t idx, void *ele);

/**
 * Remove the element at idx, shifting subsequent elements left. When dest is
 * non NULL, copy the element there and skip destroy, otherwise invoke destroy
 * when set.
 *
 * @param vec  the vector to modify
 * @param idx  zero based index of the element to remove
 * @param dest buffer of at least elesz bytes to receive the element, or NULL
 *             to invoke destroy
 * @return     0 on success, -1 if vec is NULL or idx is out of range
 */
extern int vec_remove(struct vector *vec, size_t idx, void *dest);

/**
 * Sort elements in place with qsort. Does not change the element count or call
 * destroy.
 *
 * @param vec the vector to sort
 * @param cmp comparator receiving pointers to two elements, same convention as
 *            qsort. No op if vec or cmp is NULL
 */
extern void vec_sort(struct vector *vec,
                     int (*cmp)(const void *, const void *));

/**
 * Remove all elements and reset size to zero, invoking destroy on each when
 * set. Does not release the buffer or free the vector.
 *
 * @param vec the vector to clear, or NULL
 */
extern void vec_clear(struct vector *vec);

/**
 * Allocate and initialize an iterator positioned at the first element of vec.
 *
 * @param iter output pointer to receive the new iterator, must not be NULL
 * @param vec  the vector to traverse, must not be NULL
 * @return     0 on success, -1 on failure
 */
extern int vec_iter_init(struct vector_iter **iter, struct vector *vec);

/**
 * Advance the iterator forward by one index.
 *
 * @param iter the iterator to advance, or NULL
 */
extern void vec_iter_inc(struct vector_iter *iter);

/**
 * Move the iterator backward by one index.
 *
 * @param iter the iterator to move, or NULL
 */
extern void vec_iter_dec(struct vector_iter *iter);

/**
 * Return a pointer to the element at the current iterator position.
 *
 * @param iter the iterator to read from, or NULL
 * @return     pointer to the current element, or NULL if iter is NULL or the
 *             index is out of range
 */
extern void *vec_iter_get(struct vector_iter *iter);

/**
 * Free the iterator.
 *
 * @param iter the iterator to free, or NULL
 */
extern void vec_iter_free(struct vector_iter *iter);

#ifdef __cplusplus
}
#endif

#endif
