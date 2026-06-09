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

#include <slist.h>
#include <stdlib.h>
#include <string.h>

static struct slist_node *create_node(void *data)
{
  struct slist_node *node = malloc(sizeof(struct slist_node));
  if (!node)
    return NULL;
  node->data = data;
  node->next = NULL;
  return node;
}

int slist_init(struct slist *slist, void (*destroy)(void *))
{
  if (!slist)
    return -1;
  memset(slist, 0, sizeof(struct slist));
  slist->destroy = destroy;
  return 0;
}

void slist_fini(struct slist *slist)
{
  if (!slist)
    return;
  slist_clear(slist);
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

int slist_insertn(struct slist *slist, struct slist_node *node, void *data)
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

int slist_removen(struct slist *slist, struct slist_node *node, void **dest)
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

int slist_iter_init(struct slist_iter *iter, struct slist *slist)
{
  if (!iter || !slist)
    return -1;
  iter->slist = slist;
  iter->node = slist->head;
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