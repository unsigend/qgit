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

#ifndef COL_SLIST_H
#define COL_SLIST_H

#include <stddef.h>

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

#define slist_empty(slist)                                                     \
  ((slist)->len == 0)                    /* Check if the slist is empty        \
                                          */
#define slist_size(slist) ((slist)->len) /* Get the size of the slist */
#define slist_front(slist)                                                     \
  ((slist)->head ? (slist)->head->data                                         \
                 : NULL) /* Get the front element of the slist */
#define slist_back(slist)                                                      \
  ((slist)->tail ? (slist)->tail->data                                         \
                 : NULL) /* Get the back element of the slist */
#define slist_next(node)                                                       \
  ((node) ? (node)->next : NULL) /* Get the next node of the given node */
#define slist_data(node)                                                       \
  ((node) ? (node)->data : NULL) /* Get the data of the given node */

int slist_init(struct slist *slist, void (*destroy)(void *));
void slist_fini(struct slist *slist);

int slist_pushfront(struct slist *slist, void *data);
int slist_pushback(struct slist *slist, void *data);
int slist_popfront(struct slist *slist, void **dest);

/* Operations on the node after the given node, NULL to insert or remove at the
   head */
int slist_insertn(struct slist *slist, struct slist_node *node, void *data);
int slist_removen(struct slist *slist, struct slist_node *node, void **dest);

void slist_clear(struct slist *slist);

struct slist_iter {
  struct slist *slist;
  struct slist_node *node;
};

int slist_iter_init(struct slist_iter *iter, struct slist *slist);
void slist_iter_inc(struct slist_iter *iter);
void *slist_iter_get(struct slist_iter *iter);

#endif