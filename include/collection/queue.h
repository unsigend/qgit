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

#ifndef COL_QUEUE_H
#define COL_QUEUE_H

#include <stddef.h>

#include "collection/deque.h"

struct queue;

#define queue_empty(queue)                                                     \
  deq_empty(&(queue)->deq) /* Check if the queue is empty */
#define queue_size(queue)                                                      \
  deq_size(&(queue)->deq) /* Get the size of the queue */

int queue_init(struct queue *queue, size_t elesz, void (*destroy)(void *));
void queue_fini(struct queue *queue);
int queue_enq(struct queue *queue, void *ele);
int queue_deq(struct queue *queue, void *dest);
void *queue_peek(struct queue *queue);
void queue_clear(struct queue *queue);

struct queue {
  struct deque deq;
};

#endif