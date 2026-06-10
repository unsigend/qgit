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

#include "collection/queue.h"
#include "collection/deque.h"

int queue_init(struct queue *queue, size_t elesz, void (*destroy)(void *))
{
  if (!queue || !elesz)
    return -1;
  return deq_init(&queue->deq, elesz, destroy);
}

void queue_fini(struct queue *queue)
{
  if (!queue)
    return;
  deq_fini(&queue->deq);
}

int queue_enq(struct queue *queue, void *ele)
{
  if (!queue || !ele)
    return -1;
  return deq_pushback(&queue->deq, ele);
}

int queue_deq(struct queue *queue, void *dest)
{
  if (!queue)
    return -1;
  return deq_popfront(&queue->deq, dest);
}

void *queue_peek(struct queue *queue)
{
  if (!queue)
    return NULL;
  return deq_front(&queue->deq);
}

void queue_clear(struct queue *queue)
{
  if (!queue)
    return;
  deq_clear(&queue->deq);
}