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

#include <collection/slist.h>
#include <stdlib.h>

struct slist {
    struct slist_node *head;
    struct slist_node *tail;
    size_t len;
    void (*destroy)(void *);
};

struct slist_node {
    void *data;
    struct slist_node *next;
};

struct slist_iter {
    struct slist *slist;
    struct slist_node *node;
};

static struct slist_node *create_node(void *data);

int slist_init(struct slist **slist, void (*destroy)(void *))
{
    if (!slist)
        return -1;
    struct slist *s = calloc(1, sizeof(struct slist));
    if (!s)
        return -1;
    s->destroy = destroy;
    *slist = s;
    return 0;
}

void slist_free(struct slist *slist)
{
    if (!slist)
        return;
    slist_clear(slist);
    free(slist);
}

int slist_empty(const struct slist *slist)
{
    if (!slist)
        return 1;
    return slist->len == 0;
}

size_t slist_size(const struct slist *slist)
{
    if (!slist)
        return 0;
    return slist->len;
}

void *slist_front(const struct slist *slist)
{
    if (!slist)
        return NULL;
    return slist->head ? slist->head->data : NULL;
}

void *slist_back(const struct slist *slist)
{
    if (!slist)
        return NULL;
    return slist->tail ? slist->tail->data : NULL;
}

struct slist_node *slist_head(const struct slist *slist)
{
    if (!slist)
        return NULL;
    return slist->head;
}

struct slist_node *slist_next(const struct slist_node *node)
{
    if (!node)
        return NULL;
    return node->next;
}

void *slist_data(const struct slist_node *node)
{
    if (!node)
        return NULL;
    return node->data;
}

int slist_pushfront(struct slist *slist, void *data)
{
    if (!slist || !data)
        return -1;
    struct slist_node *node = create_node(data);
    if (!node)
        return -1;
    node->next = slist->head;
    slist->head = node;
    if (!slist->tail)
        slist->tail = node;
    slist->len++;
    return 0;
}

int slist_pushback(struct slist *slist, void *data)
{
    if (!slist || !data)
        return -1;
    struct slist_node *node = create_node(data);
    if (!node)
        return -1;
    if (slist->tail)
        slist->tail->next = node;
    else
        slist->head = node;
    slist->tail = node;
    slist->len++;
    return 0;
}

int slist_popfront(struct slist *slist, void **dest)
{
    if (!slist || slist_empty(slist))
        return -1;

    if (dest)
        *dest = slist_front(slist);
    else if (slist->destroy)
        slist->destroy(slist_front(slist));
    struct slist_node *node = slist->head;
    slist->head = node->next;
    if (!slist->head)
        slist->tail = NULL;
    free(node);
    slist->len--;
    return 0;
}

int slist_insert_next(struct slist *slist, struct slist_node *node, void *data)
{
    if (!slist || !data)
        return -1;
    if (!node)
        return slist_pushfront(slist, data);
    struct slist_node *new_node = create_node(data);
    if (!new_node)
        return -1;
    new_node->next = node->next;
    node->next = new_node;
    if (!new_node->next)
        slist->tail = new_node;
    slist->len++;
    return 0;
}

int slist_remove_next(struct slist *slist, struct slist_node *node, void **dest)
{
    if (!slist)
        return -1;
    if (!node)
        return slist_popfront(slist, dest);
    struct slist_node *next = node->next;
    if (!next)
        return -1;
    if (dest)
        *dest = next->data;
    else if (slist->destroy)
        slist->destroy(next->data);
    node->next = next->next;
    if (!node->next)
        slist->tail = node;
    free(next);
    slist->len--;
    return 0;
}

void slist_clear(struct slist *slist)
{
    if (!slist)
        return;
    struct slist_node *node = slist->head;
    while (node) {
        struct slist_node *next = node->next;
        if (slist->destroy)
            slist->destroy(node->data);
        free(node);
        node = next;
    }
    slist->head = NULL;
    slist->tail = NULL;
    slist->len = 0;
}

int slist_iter_init(struct slist_iter **iter, struct slist *slist)
{
    if (!iter || !slist)
        return -1;
    struct slist_iter *i = calloc(1, sizeof(struct slist_iter));
    if (!i)
        return -1;
    i->slist = slist;
    i->node = slist->head;
    *iter = i;
    return 0;
}

void slist_iter_inc(struct slist_iter *iter)
{
    if (!iter || !iter->node)
        return;
    iter->node = iter->node->next;
}

void *slist_iter_get(struct slist_iter *iter)
{
    if (!iter)
        return NULL;
    return slist_data(iter->node);
}

void slist_iter_free(struct slist_iter *iter)
{
    if (!iter)
        return;
    free(iter);
}

static struct slist_node *create_node(void *data)
{
    struct slist_node *node = malloc(sizeof(struct slist_node));
    if (!node)
        return NULL;
    node->data = data;
    node->next = NULL;
    return node;
}
