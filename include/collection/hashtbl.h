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

#ifndef COL_HASHTBL_H
#define COL_HASHTBL_H

#include <stddef.h>
#include <stdint.h>

struct hashtbl_node {
  void *key;
  void *val;
  struct hashtbl_node *next;
};

struct hashtbl_fns {
  uint32_t (*hash)(void *);
  int (*cmp)(void *, void *);
  void (*destroy_key)(void *);
  void (*destroy_val)(void *);
};

struct hashtbl {
  struct hashtbl_node **buckets;
  size_t bucketsz;
  size_t sz;
  float threshold; /* max load factor */
  struct hashtbl_fns *fns;
};

#define hashtbl_empty(ht) ((ht)->sz == 0) /* Check if the hashtbl is empty */
#define hashtbl_size(ht) ((ht)->sz)       /* Size of the hashtbl */
#define hashtbl_threshold(ht) ((ht)->threshold) /* Threshold of the hashtbl */
#define hashtbl_bucketsz(ht) ((ht)->bucketsz)   /* bucket size */
#define hashtbl_buckets(ht) ((ht)->buckets)     /* raw buckets */
#define hashtbl_fns(ht) ((ht)->fns)             /* fns */

int hashtbl_init(struct hashtbl *ht, struct hashtbl_fns *fns);
void hashtbl_fini(struct hashtbl *ht);

int hashtbl_setthreshold(struct hashtbl *ht, float threshold, float *old);
float hashtbl_loadfactor(struct hashtbl *ht);

/* Insert a new key-value pair into the hash table. Returns 0 on success, -1 on
   error or if the key already exists */
int hashtbl_insert(struct hashtbl *ht, void *key, void *val);

/* Update the value of the given key. Returns 0 on success, -1 on error or if
   the key does not exist */
int hashtbl_update(struct hashtbl *ht, void *key, void *newval, void **dest);

int hashtbl_remove(struct hashtbl *ht, void *key, void **dest);

void *hashtbl_find(struct hashtbl *ht, void *key);
struct hashtbl_node *hashtbl_findnode(struct hashtbl *ht, void *key);

void hashtbl_clear(struct hashtbl *ht);

struct hashtbl_iter {
  struct hashtbl *ht;
  size_t bucket;
  struct hashtbl_node *node;
};

int hashtbl_iter_init(struct hashtbl_iter *iter, struct hashtbl *ht);
void hashtbl_iter_inc(struct hashtbl_iter *iter);
struct hashtbl_node *hashtbl_iter_get(struct hashtbl_iter *iter);

#endif