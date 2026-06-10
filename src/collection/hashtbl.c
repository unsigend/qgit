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

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "collection/hashtbl.h"

#define THRESHOLD 0.75f
#define NBUCKETS 16
#define GROWFACTOR 2

static inline struct hashtbl_node *node_create(void *key, void *val);
static inline struct hashtbl_node **buckets_create(size_t bucketsz);

static int rehash(struct hashtbl *ht);
static inline size_t hashidx(uint32_t hash, size_t bucketsz);
#define need_rehash(ht) (hashtbl_loadfactor(ht) >= hashtbl_threshold(ht))

int hashtbl_init(struct hashtbl *ht, struct hashtbl_fns *fns)
{
  if (!ht || !fns || !fns->hash || !fns->cmp)
    return -1;
  memset(ht, 0, sizeof(struct hashtbl));
  ht->fns = fns;
  ht->threshold = THRESHOLD;
  return 0;
}

int hashtbl_setthreshold(struct hashtbl *ht, float threshold, float *old)
{
  if (!ht || threshold <= 0.0f)
    return -1;
  if (old)
    *old = ht->threshold;
  ht->threshold = threshold;
  return 0;
}

float hashtbl_loadfactor(struct hashtbl *ht)
{
  if (!ht || !ht->bucketsz)
    return 0.0f;
  return (float)ht->sz / (float)ht->bucketsz;
}

void hashtbl_fini(struct hashtbl *ht)
{
  if (!ht)
    return;
  hashtbl_clear(ht);
  free(ht->buckets);
  memset(ht, 0, sizeof(struct hashtbl));
}

void hashtbl_clear(struct hashtbl *ht)
{
  if (!ht || hashtbl_empty(ht))
    return;
  for (size_t i = 0; i < ht->bucketsz; i++) {
    struct hashtbl_node *node = ht->buckets[i];
    while (node) {
      struct hashtbl_node *next = node->next;
      if (ht->fns->destroy_key)
        ht->fns->destroy_key(node->key);
      if (ht->fns->destroy_val)
        ht->fns->destroy_val(node->val);
      free(node);
      node = next;
    }
  }
  ht->sz = 0;
  memset(ht->buckets, 0, ht->bucketsz * sizeof(struct hashtbl_node *));
}

int hashtbl_insert(struct hashtbl *ht, void *key, void *val)
{
  if (!ht || !key)
    return -1;
  if (hashtbl_findnode(ht, key))
    return -1;
  if (!hashtbl_bucketsz(ht)) {
    ht->bucketsz = NBUCKETS;
    ht->buckets = buckets_create(ht->bucketsz);
    if (!ht->buckets)
      return -1;
  }
  if (need_rehash(ht) && rehash(ht) == -1)
    return -1;
  uint32_t hash = ht->fns->hash(key);
  size_t idx = hashidx(hash, ht->bucketsz);
  struct hashtbl_node *node = node_create(key, val);
  if (!node)
    return -1;
  node->next = ht->buckets[idx];
  ht->buckets[idx] = node;
  ht->sz++;
  return 0;
}

int hashtbl_update(struct hashtbl *ht, void *key, void *newval, void **dest)
{
  if (!ht || !key || !newval)
    return -1;
  struct hashtbl_node *node = hashtbl_findnode(ht, key);
  if (!node)
    return -1;
  if (dest)
    *dest = node->val;
  else if (ht->fns->destroy_val)
    ht->fns->destroy_val(node->val);
  node->val = newval;
  return 0;
}

int hashtbl_remove(struct hashtbl *ht, void *key, void **dest)
{
  if (!ht || !key || hashtbl_empty(ht))
    return -1;
  uint32_t hash = ht->fns->hash(key);
  size_t idx = hashidx(hash, ht->bucketsz);
  struct hashtbl_node *node = ht->buckets[idx];
  if (!node)
    return -1;

  struct hashtbl_node *prev = NULL;
  while (node) {
    if (ht->fns->cmp(node->key, key) == 0) {
      if (dest)
        *dest = node->val;
      else if (ht->fns->destroy_val)
        ht->fns->destroy_val(node->val);

      if (ht->fns->destroy_key)
        ht->fns->destroy_key(node->key);

      if (prev)
        prev->next = node->next;
      else
        ht->buckets[idx] = node->next;
      free(node);
      ht->sz--;
      return 0;
    }
    prev = node;
    node = node->next;
  }

  return -1;
}

void *hashtbl_find(struct hashtbl *ht, void *key)
{
  struct hashtbl_node *node = hashtbl_findnode(ht, key);
  return node ? node->val : NULL;
}

struct hashtbl_node *hashtbl_findnode(struct hashtbl *ht, void *key)
{
  if (!ht || !key || hashtbl_empty(ht))
    return NULL;
  uint32_t hash = ht->fns->hash(key);
  size_t idx = hashidx(hash, ht->bucketsz);
  struct hashtbl_node *node = ht->buckets[idx];
  while (node) {
    if (ht->fns->cmp(node->key, key) == 0)
      return node;
    node = node->next;
  }
  return NULL;
}

static struct hashtbl_node *node_create(void *key, void *val)
{
  struct hashtbl_node *node = calloc(1, sizeof(struct hashtbl_node));
  if (!node)
    return NULL;
  node->key = key;
  node->val = val;
  return node;
}

static inline struct hashtbl_node **buckets_create(size_t bucketsz)
{
  return calloc(bucketsz, sizeof(struct hashtbl_node *));
}

static inline size_t hashidx(uint32_t hash, size_t bucketsz)
{
  return hash & (bucketsz - 1);
}

static int rehash(struct hashtbl *ht)
{
  size_t newbucketsz = ht->bucketsz ? ht->bucketsz * GROWFACTOR : NBUCKETS;
  struct hashtbl_node **newbuckets = buckets_create(newbucketsz);
  if (!newbuckets)
    return -1;

  for (size_t i = 0; i < ht->bucketsz; i++) {
    struct hashtbl_node *node = ht->buckets[i];
    while (node) {
      struct hashtbl_node *next = node->next;
      size_t newidx = hashidx(ht->fns->hash(node->key), newbucketsz);
      node->next = newbuckets[newidx];
      newbuckets[newidx] = node;
      node = next;
    }
    ht->buckets[i] = NULL;
  }

  free(ht->buckets);
  ht->buckets = newbuckets;
  ht->bucketsz = newbucketsz;
  return 0;
}

int hashtbl_iter_init(struct hashtbl_iter *iter, struct hashtbl *ht)
{
  if (!iter || !ht)
    return -1;
  iter->ht = ht;
  iter->bucket = 0;
  iter->node = ht->buckets ? ht->buckets[0] : NULL;
  return 0;
}

void hashtbl_iter_inc(struct hashtbl_iter *iter)
{
  if (!iter || !iter->node)
    return;
  iter->node = iter->node->next;
  while (!iter->node && iter->bucket < iter->ht->bucketsz - 1) {
    iter->bucket++;
    iter->node = iter->ht->buckets[iter->bucket];
  }
}

struct hashtbl_node *hashtbl_iter_get(struct hashtbl_iter *iter)
{
  if (!iter)
    return NULL;
  return iter->node;
}