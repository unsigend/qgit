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

#include <collection/hashtbl.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

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
    struct hashtbl_fns fns;
};

struct hashtbl_iter {
    struct hashtbl *ht;
    size_t bucket;
    struct hashtbl_node *node;
};

#define THRESHOLD 0.75f
#define NBUCKETS 16
#define GROWFACTOR 2

#define need_rehash(table)                                                     \
    (hashtbl_loadfactor(table) >= hashtbl_threshold(table))

static inline struct hashtbl_node *node_create(void *key, void *val);
static inline struct hashtbl_node **buckets_create(size_t bucketsz);
static int rehash(struct hashtbl *table);
static inline size_t hashidx(uint32_t hash, size_t bucketsz);

int hashtbl_init(struct hashtbl **table, hashtbl_fns_hash hash,
                 hashtbl_fns_cmp cmp, hashtbl_fns_destroy_key destroy_key,
                 hashtbl_fns_destroy_val destroy_val)
{
    if (!table || !hash || !cmp)
        return -1;
    *table = NULL;
    struct hashtbl *t = calloc(1, sizeof(struct hashtbl));
    if (!t)
        return -1;
    t->fns.hash = hash;
    t->fns.cmp = cmp;
    t->fns.destroy_key = destroy_key;
    t->fns.destroy_val = destroy_val;
    t->threshold = THRESHOLD;
    *table = t;
    return 0;
}

void hashtbl_free(struct hashtbl *table)
{
    if (!table)
        return;
    hashtbl_clear(table);
    free(table->buckets);
    free(table);
}

int hashtbl_empty(const struct hashtbl *table)
{
    return table ? table->sz == 0 : 1;
}

size_t hashtbl_size(const struct hashtbl *table)
{
    return table ? table->sz : 0;
}

float hashtbl_threshold(const struct hashtbl *table)
{
    return table ? table->threshold : 0.0f;
}

size_t hashtbl_bucketsz(const struct hashtbl *table)
{
    return table ? table->bucketsz : 0;
}

float hashtbl_loadfactor(const struct hashtbl *table)
{
    if (!table || !table->bucketsz)
        return 0.0f;
    return (float)table->sz / (float)table->bucketsz;
}

void *hashtbl_find(const struct hashtbl *table, void *key)
{
    return hashtbl_node_val(hashtbl_findnode(table, key));
}

struct hashtbl_node *hashtbl_findnode(const struct hashtbl *table, void *key)
{
    if (!table || !key || hashtbl_empty(table))
        return NULL;
    uint32_t hash = table->fns.hash(key);
    size_t idx = hashidx(hash, table->bucketsz);
    struct hashtbl_node *node = table->buckets[idx];
    while (node) {
        if (table->fns.cmp(node->key, key) == 0)
            return node;
        node = node->next;
    }
    return NULL;
}

void *hashtbl_node_key(const struct hashtbl_node *node)
{
    return node ? node->key : NULL;
}

void *hashtbl_node_val(const struct hashtbl_node *node)
{
    return node ? node->val : NULL;
}

int hashtbl_setthreshold(struct hashtbl *table, float threshold, float *old)
{
    if (!table || threshold <= 0.0f)
        return -1;
    if (old)
        *old = table->threshold;
    table->threshold = threshold;
    return 0;
}

int hashtbl_insert(struct hashtbl *table, void *key, void *val)
{
    if (!table || !key)
        return -1;
    if (hashtbl_findnode(table, key))
        return -1;
    if (!hashtbl_bucketsz(table)) {
        table->bucketsz = NBUCKETS;
        table->buckets = buckets_create(table->bucketsz);
        if (!table->buckets)
            return -1;
    }
    if (need_rehash(table) && rehash(table) == -1)
        return -1;
    uint32_t hash = table->fns.hash(key);
    size_t idx = hashidx(hash, table->bucketsz);
    struct hashtbl_node *node = node_create(key, val);
    if (!node)
        return -1;
    node->next = table->buckets[idx];
    table->buckets[idx] = node;
    table->sz++;
    return 0;
}

int hashtbl_update(struct hashtbl *table, void *key, void *newval, void **dest)
{
    if (!table || !key || !newval)
        return -1;
    struct hashtbl_node *node = hashtbl_findnode(table, key);
    if (!node)
        return -1;
    if (dest)
        *dest = node->val;
    else if (table->fns.destroy_val)
        table->fns.destroy_val(node->val);
    node->val = newval;
    return 0;
}

int hashtbl_remove(struct hashtbl *table, void *key, void **dest)
{
    if (!table || !key || hashtbl_empty(table))
        return -1;
    uint32_t hash = table->fns.hash(key);
    size_t idx = hashidx(hash, table->bucketsz);
    struct hashtbl_node *node = table->buckets[idx];
    if (!node)
        return -1;

    struct hashtbl_node *prev = NULL;
    while (node) {
        if (table->fns.cmp(node->key, key) == 0) {
            if (dest)
                *dest = node->val;
            else if (table->fns.destroy_val)
                table->fns.destroy_val(node->val);

            if (table->fns.destroy_key)
                table->fns.destroy_key(node->key);

            if (prev)
                prev->next = node->next;
            else
                table->buckets[idx] = node->next;
            free(node);
            table->sz--;
            return 0;
        }
        prev = node;
        node = node->next;
    }

    return -1;
}

void hashtbl_clear(struct hashtbl *table)
{
    if (!table || hashtbl_empty(table))
        return;
    for (size_t i = 0; i < table->bucketsz; i++) {
        struct hashtbl_node *node = table->buckets[i];
        while (node) {
            struct hashtbl_node *next = node->next;
            if (table->fns.destroy_key)
                table->fns.destroy_key(node->key);
            if (table->fns.destroy_val)
                table->fns.destroy_val(node->val);
            free(node);
            node = next;
        }
    }
    table->sz = 0;
    memset(table->buckets, 0, table->bucketsz * sizeof(struct hashtbl_node *));
}

int hashtbl_iter_init(struct hashtbl_iter **iter, struct hashtbl *table)
{
    if (!iter || !table)
        return -1;
    *iter = NULL;
    struct hashtbl_iter *i = calloc(1, sizeof(struct hashtbl_iter));
    if (!i)
        return -1;
    i->ht = table;
    i->bucket = 0;
    i->node = table->buckets ? table->buckets[0] : NULL;
    *iter = i;
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

struct hashtbl_node *hashtbl_iter_get(const struct hashtbl_iter *iter)
{
    if (!iter)
        return NULL;
    return iter->node;
}

void hashtbl_iter_free(struct hashtbl_iter *iter)
{
    if (!iter)
        return;
    free(iter);
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

static int rehash(struct hashtbl *table)
{
    size_t newbucketsz =
        table->bucketsz ? table->bucketsz * GROWFACTOR : NBUCKETS;
    struct hashtbl_node **newbuckets = buckets_create(newbucketsz);
    if (!newbuckets)
        return -1;

    for (size_t i = 0; i < table->bucketsz; i++) {
        struct hashtbl_node *node = table->buckets[i];
        while (node) {
            struct hashtbl_node *next = node->next;
            size_t newidx = hashidx(table->fns.hash(node->key), newbucketsz);
            node->next = newbuckets[newidx];
            newbuckets[newidx] = node;
            node = next;
        }
        table->buckets[i] = NULL;
    }

    free(table->buckets);
    table->buckets = newbuckets;
    table->bucketsz = newbucketsz;
    return 0;
}
