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
#include <collection/set.h>
#include <stdlib.h>

struct set {
    struct hashtbl *tbl;
};

struct set_iter {
    struct hashtbl_iter *iter;
};

int set_init(struct set **set, set_fns_hash hash, set_fns_cmp cmp,
             set_fns_destroy destroy)
{
    if (!set || !hash || !cmp)
        return -1;

    *set = NULL;
    struct set *s = calloc(1, sizeof(struct set));
    if (!s)
        return -1;
    if (hashtbl_init(&s->tbl, hash, cmp, destroy, NULL) == -1) {
        free(s);
        return -1;
    }
    *set = s;
    return 0;
}

void set_free(struct set *set)
{
    if (!set)
        return;
    hashtbl_free(set->tbl);
    free(set);
}

int set_empty(const struct set *set)
{
    return set ? hashtbl_empty(set->tbl) : 1;
}

size_t set_size(const struct set *set)
{
    return set ? hashtbl_size(set->tbl) : 0;
}

int set_contains(const struct set *set, void *ele)
{
    if (!set || !ele)
        return 0;
    return hashtbl_findnode(set->tbl, ele) != NULL;
}

int set_insert(struct set *set, void *ele)
{
    if (!set || !ele)
        return -1;

    if (set_contains(set, ele))
        return 0;

    return hashtbl_insert(set->tbl, ele, ele);
}

int set_remove(struct set *set, void *ele)
{
    if (!set || !ele)
        return -1;

    if (!set_contains(set, ele))
        return 0;

    return hashtbl_remove(set->tbl, ele, NULL);
}

void set_clear(struct set *set)
{
    if (!set)
        return;
    hashtbl_clear(set->tbl);
}

int set_iter_init(struct set_iter **iter, struct set *set)
{
    if (!iter || !set)
        return -1;
    *iter = NULL;
    struct set_iter *i = calloc(1, sizeof(struct set_iter));
    if (!i)
        return -1;
    if (hashtbl_iter_init(&i->iter, set->tbl) == -1) {
        free(i);
        return -1;
    }
    *iter = i;
    return 0;
}

void set_iter_inc(struct set_iter *iter)
{
    if (!iter)
        return;
    hashtbl_iter_inc(iter->iter);
}

void *set_iter_get(const struct set_iter *iter)
{
    if (!iter)
        return NULL;
    const struct hashtbl_node *node = hashtbl_iter_get(iter->iter);
    if (!node)
        return NULL;
    return hashtbl_node_key(node);
}

void set_iter_free(struct set_iter *iter)
{
    if (!iter)
        return;
    hashtbl_iter_free(iter->iter);
    free(iter);
}
