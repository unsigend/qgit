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

#ifndef COLLECTION_SLIST_H
#define COLLECTION_SLIST_H

#include <stddef.h>

struct slist;
struct slist_node;
struct slist_iter;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Allocate and initialize an empty list with an optional destructor invoked on
 * each stored data pointer when it is discarded.
 *
 * @param slist   output pointer to receive the new list, must not be NULL
 * @param destroy optional callback invoked on each data pointer when it is
 *                discarded, or NULL for no op
 * @return        0 on success, -1 on failure
 */
extern int slist_init(struct slist **slist, void (*destroy)(void *));

/**
 * Destroy all elements, release every node, and free the list.
 *
 * @param slist the list to free, or NULL
 */
extern void slist_free(struct slist *slist);

/**
 * Test whether the list contains no elements.
 *
 * @param slist the list to inspect, or NULL
 * @return      non zero if empty, zero otherwise
 */
extern int slist_empty(const struct slist *slist);

/**
 * Return the current number of elements in the list.
 *
 * @param slist the list to inspect, or NULL
 * @return      element count, or 0 if slist is NULL
 */
extern size_t slist_size(const struct slist *slist);

/**
 * Return the data pointer stored in the first node.
 *
 * @param slist the list to inspect, or NULL
 * @return      front data pointer, or NULL if slist is NULL or empty
 */
extern void *slist_front(const struct slist *slist);

/**
 * Return the data pointer stored in the last node.
 *
 * @param slist the list to inspect, or NULL
 * @return      back data pointer, or NULL if slist is NULL or empty
 */
extern void *slist_back(const struct slist *slist);

/**
 * Return the first node in the list.
 *
 * @param slist the list to inspect, or NULL
 * @return      head node, or NULL if slist is NULL or empty
 */
extern struct slist_node *slist_head(const struct slist *slist);

/**
 * Return the next node after node.
 *
 * @param node the node to inspect, or NULL
 * @return     next node, or NULL if node is NULL or has no successor
 */
extern struct slist_node *slist_next(const struct slist_node *node);

/**
 * Return the data pointer stored in node.
 *
 * @param node the node to inspect, or NULL
 * @return     data pointer, or NULL if node is NULL
 */
extern void *slist_data(const struct slist_node *node);

/**
 * Prepend a data pointer to the front of the list.
 *
 * @param slist the list to modify
 * @param data  pointer to store, must not be NULL
 * @return      0 on success, -1 on failure
 */
extern int slist_pushfront(struct slist *slist, void *data);

/**
 * Append a data pointer to the back of the list.
 *
 * @param slist the list to modify
 * @param data  pointer to store, must not be NULL
 * @return      0 on success, -1 on failure
 */
extern int slist_pushback(struct slist *slist, void *data);

/**
 * Remove the front node. When dest is non NULL, store the data pointer in
 * *dest and skip destroy, otherwise invoke destroy when set.
 *
 * @param slist the list to modify
 * @param dest  output location for the front data pointer, or NULL to invoke
 *              destroy
 * @return      0 on success, -1 if slist is NULL or the list is empty
 */
extern int slist_popfront(struct slist *slist, void **dest);

/**
 * Insert a new node carrying data immediately after node. When node is NULL,
 * behaves like slist_pushfront.
 *
 * @param slist the list to modify
 * @param node  predecessor node, or NULL to insert at the head
 * @param data  pointer to store, must not be NULL
 * @return      0 on success, -1 on failure
 */
extern int slist_insert_next(struct slist *slist, struct slist_node *node,
                             void *data);

/**
 * Remove the node immediately after node. When node is NULL, behaves like
 * slist_popfront. When dest is non NULL, store the removed data pointer in
 * *dest and skip destroy, otherwise invoke destroy when set.
 *
 * @param slist the list to modify
 * @param node  predecessor node, or NULL to remove the head
 * @param dest  output location for the removed data pointer, or NULL to invoke
 *              destroy
 * @return      0 on success, -1 if slist is NULL, node has no successor, or
 *              the operation is invalid
 */
extern int slist_remove_next(struct slist *slist, struct slist_node *node,
                             void **dest);

/**
 * Remove all nodes and reset size to zero, invoking destroy on each data
 * pointer when set. Does not free the list itself. No op if slist is NULL.
 *
 * @param slist the list to clear, or NULL
 */
extern void slist_clear(struct slist *slist);

/**
 * Allocate and initialize an iterator at the first node of slist.
 *
 * @param iter  output pointer to receive the new iterator, must not be NULL
 * @param slist the list to traverse, must not be NULL
 * @return      0 on success, -1 on failure
 */
extern int slist_iter_init(struct slist_iter **iter, struct slist *slist);

/**
 * Advance the iterator to the next node.
 *
 * @param iter the iterator to advance, or NULL
 */
extern void slist_iter_inc(struct slist_iter *iter);

/**
 * Return the data pointer at the current iterator position.
 *
 * @param iter the iterator to read from, or NULL
 * @return     data pointer, or NULL if iter is NULL or past the last node
 */
extern void *slist_iter_get(struct slist_iter *iter);

/**
 * Free the iterator.
 *
 * @param iter the iterator to free, or NULL
 */
extern void slist_iter_free(struct slist_iter *iter);

#ifdef __cplusplus
}
#endif

#endif
