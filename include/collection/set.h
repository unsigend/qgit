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

#ifndef COLLECTION_SET_H
#define COLLECTION_SET_H

#include <stddef.h>
#include <stdint.h>

struct set;
struct set_iter;

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t (*set_fns_hash)(void *);
typedef int (*set_fns_cmp)(void *, void *);
typedef void (*set_fns_destroy)(void *);

/**
 * Allocate and initialize an empty set.
 *
 * Elements are stored as opaque pointers. The set does not copy pointed to
 * data. destroy is invoked when a stored pointer is discarded, or NULL for
 * no op.
 *
 * @param set     output pointer to receive the new set, must not be NULL
 * @param hash    element hash function, must not be NULL
 * @param cmp     element comparator, must not be NULL
 * @param destroy optional callback invoked on each discarded element, or NULL
 * @return        0 on success, -1 on failure
 */
extern int set_init(struct set **set, set_fns_hash hash, set_fns_cmp cmp,
                    set_fns_destroy destroy);

/**
 * Destroy all elements and free the set.
 *
 * @param set the set to free, or NULL
 */
extern void set_free(struct set *set);

/**
 * Test whether the set contains no elements.
 *
 * @param set the set to inspect, or NULL
 * @return    non zero if empty, zero otherwise
 */
extern int set_empty(const struct set *set);

/**
 * Return the current number of elements in the set.
 *
 * @param set the set to inspect, or NULL
 * @return    element count, or 0 if set is NULL
 */
extern size_t set_size(const struct set *set);

/**
 * Test whether the set contains ele.
 *
 * @param set the set to search, or NULL
 * @param ele element to find, must not be NULL
 * @return    non zero if present, zero if set is NULL, ele is NULL, or ele is
 *            not present
 */
extern int set_contains(const struct set *set, void *ele);

/**
 * Insert ele into the set. If ele is already present, does nothing and returns
 * 0.
 *
 * @param set the set to modify
 * @param ele element pointer to store, must not be NULL
 * @return    0 on success, -1 on error
 */
extern int set_insert(struct set *set, void *ele);

/**
 * Remove ele from the set. If ele is not present, does nothing and returns 0.
 *
 * @param set the set to modify
 * @param ele element to remove, must not be NULL
 * @return    0 on success, -1 on error
 */
extern int set_remove(struct set *set, void *ele);

/**
 * Remove all elements and reset size to zero, invoking destroy on each stored
 * pointer when set. Does not free the set itself. No op if set is NULL.
 *
 * @param set the set to clear, or NULL
 */
extern void set_clear(struct set *set);

/**
 * Allocate and initialize an iterator at the first element in set.
 *
 * @param iter output pointer to receive the new iterator, must not be NULL
 * @param set  the set to traverse, must not be NULL
 * @return     0 on success, -1 on failure
 */
extern int set_iter_init(struct set_iter **iter, struct set *set);

/**
 * Advance the iterator to the next element.
 *
 * @param iter the iterator to advance, or NULL
 */
extern void set_iter_inc(struct set_iter *iter);

/**
 * Return the element pointer at the current iterator position.
 *
 * @param iter the iterator to read from, or NULL
 * @return     element pointer, or NULL if iter is NULL or not on an element
 */
extern void *set_iter_get(const struct set_iter *iter);

/**
 * Free the iterator.
 *
 * @param iter the iterator to free, or NULL
 */
extern void set_iter_free(struct set_iter *iter);

#ifdef __cplusplus
}
#endif

#endif
