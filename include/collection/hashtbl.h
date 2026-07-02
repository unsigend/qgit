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

#ifndef COLLECTION_HASHTBL_H
#define COLLECTION_HASHTBL_H

#include <stddef.h>
#include <stdint.h>

struct hashtbl;
struct hashtbl_node;
struct hashtbl_iter;

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t (*hashtbl_fns_hash)(void *);
typedef int (*hashtbl_fns_cmp)(void *, void *);
typedef void (*hashtbl_fns_destroy_key)(void *);
typedef void (*hashtbl_fns_destroy_val)(void *);

/**
 * Allocate and initialize an empty hash table.
 *
 * Keys and values are stored as opaque pointers. The table does not copy
 * pointed to data. destroy_key and destroy_val are invoked when a stored
 * pointer is discarded, or NULL for no op.
 *
 * @param table       output pointer to receive the new table, must not be NULL
 * @param hash        key hash function, must not be NULL
 * @param cmp         key comparator, must not be NULL
 * @param destroy_key optional callback invoked on each discarded key, or NULL
 * @param destroy_val optional callback invoked on each discarded value, or NULL
 * @return            0 on success, -1 on failure
 */
extern int hashtbl_init(struct hashtbl **table, hashtbl_fns_hash hash,
                        hashtbl_fns_cmp cmp,
                        hashtbl_fns_destroy_key destroy_key,
                        hashtbl_fns_destroy_val destroy_val);

/**
 * Destroy all entries and free the table.
 *
 * @param table the table to free, or NULL
 */
extern void hashtbl_free(struct hashtbl *table);

/**
 * Test whether the table contains no entries.
 *
 * @param table the table to inspect, or NULL
 * @return      non zero if empty, zero otherwise
 */
extern int hashtbl_empty(const struct hashtbl *table);

/**
 * Return the current number of key value entries in the table.
 *
 * @param table the table to inspect, or NULL
 * @return      entry count, or 0 if table is NULL
 */
extern size_t hashtbl_size(const struct hashtbl *table);

/**
 * Return the configured maximum load factor before rehashing.
 *
 * @param table the table to inspect, or NULL
 * @return      threshold value, or 0.0f if table is NULL
 */
extern float hashtbl_threshold(const struct hashtbl *table);

/**
 * Return the current number of hash buckets.
 *
 * @param table the table to inspect, or NULL
 * @return      bucket count, or 0 if table is NULL
 */
extern size_t hashtbl_bucketsz(const struct hashtbl *table);

/**
 * Return the current load factor (entry count divided by bucket count).
 *
 * @param table the table to inspect, or NULL
 * @return      load factor, or 0.0f if table is NULL or has no buckets
 */
extern float hashtbl_loadfactor(const struct hashtbl *table);

/**
 * Look up the value associated with key.
 *
 * @param table the table to search, or NULL
 * @param key   key to find, must not be NULL
 * @return      stored value pointer, or NULL if table is NULL, key is NULL, or
 *              the key is not present
 */
extern void *hashtbl_find(const struct hashtbl *table, void *key);

/**
 * Look up the node associated with key.
 *
 * @param table the table to search, or NULL
 * @param key   key to find, must not be NULL
 * @return      matching node, or NULL if table is NULL, key is NULL, or the key
 *              is not present
 */
extern struct hashtbl_node *hashtbl_findnode(const struct hashtbl *table,
                                             void *key);

/**
 * Return the key pointer stored in node.
 *
 * @param node the node to inspect, or NULL
 * @return     stored key pointer, or NULL if node is NULL
 */
extern void *hashtbl_node_key(const struct hashtbl_node *node);

/**
 * Return the value pointer stored in node.
 *
 * @param node the node to inspect, or NULL
 * @return     stored value pointer, or NULL if node is NULL
 */
extern void *hashtbl_node_val(const struct hashtbl_node *node);

/**
 * Set the maximum load factor before rehashing.
 *
 * @param table     the table to modify
 * @param threshold new maximum load factor, must be positive
 * @param old       optional output location for the previous threshold, or NULL
 * @return          0 on success, -1 if table is NULL or threshold is invalid
 */
extern int hashtbl_setthreshold(struct hashtbl *table, float threshold,
                                float *old);

/**
 * Insert a key value pair. Fails when key is already present.
 *
 * @param table the table to modify
 * @param key   key pointer to store, must not be NULL
 * @param val   value pointer to store, may be NULL
 * @return      0 on success, -1 on error or if key already exists
 */
extern int hashtbl_insert(struct hashtbl *table, void *key, void *val);

/**
 * Replace the value for an existing key. When dest is non NULL, store the
 * previous value pointer in *dest and skip destroy_val, otherwise invoke
 * destroy_val when set.
 *
 * @param table  the table to modify
 * @param key    key to update, must not be NULL
 * @param newval new value pointer, must not be NULL
 * @param dest   output location for the previous value pointer, or NULL to
 *               invoke destroy_val
 * @return       0 on success, -1 if table is NULL, arguments are invalid, or
 *               key is not present
 */
extern int hashtbl_update(struct hashtbl *table, void *key, void *newval,
                          void **dest);

/**
 * Remove the entry for key. When dest is non NULL, store the removed value
 * pointer in *dest and skip destroy_val, otherwise invoke destroy_val when
 * set. destroy_key is always invoked on the removed key when set.
 *
 * @param table the table to modify
 * @param key   key to remove, must not be NULL
 * @param dest  output location for the removed value pointer, or NULL to invoke
 *              destroy_val
 * @return      0 on success, -1 if table is NULL, key is NULL, or key is not
 *              present
 */
extern int hashtbl_remove(struct hashtbl *table, void *key, void **dest);

/**
 * Remove all entries and reset size to zero, invoking destroy_key and
 * destroy_val on each stored pointer when set. Does not free the table itself.
 * No op if table is NULL.
 *
 * @param table the table to clear, or NULL
 */
extern void hashtbl_clear(struct hashtbl *table);

/**
 * Allocate and initialize an iterator at the first entry in table.
 *
 * @param iter  output pointer to receive the new iterator, must not be NULL
 * @param table the table to traverse, must not be NULL
 * @return      0 on success, -1 on failure
 */
extern int hashtbl_iter_init(struct hashtbl_iter **iter, struct hashtbl *table);

/**
 * Advance the iterator to the next entry.
 *
 * @param iter the iterator to advance, or NULL
 */
extern void hashtbl_iter_inc(struct hashtbl_iter *iter);

/**
 * Return the node at the current iterator position.
 *
 * @param iter the iterator to read from, or NULL
 * @return     current node, or NULL if iter is NULL or not on an entry
 */
extern struct hashtbl_node *hashtbl_iter_get(const struct hashtbl_iter *iter);

/**
 * Free the iterator.
 *
 * @param iter the iterator to free, or NULL
 */
extern void hashtbl_iter_free(struct hashtbl_iter *iter);

#ifdef __cplusplus
}
#endif

#endif
