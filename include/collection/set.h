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

#ifndef COL_SET_H
#define COL_SET_H

#include <stdint.h>

#include "collection/hashtbl.h"

struct set_fns {
  uint32_t (*hash)(void *);
  int (*cmp)(void *, void *);
  void (*destroy)(void *);
};

struct set {
  struct hashtbl tbl;
  struct hashtbl_fns tbl_fns;
};

#define set_empty(set)                                                         \
  (hashtbl_empty(&(set)->tbl)) /* Check if the set is empty */
#define set_size(set) (hashtbl_size(&(set)->tbl)) /* Size of the set */

int set_init(struct set *set, struct set_fns *fns);
void set_fini(struct set *set);

/* Insert a new element into the set. Returns 0 on success, -1 on error. If the
   element already exists, ignore and return 0. */
int set_insert(struct set *set, void *ele);

/* Remove an element from the set. Returns 0 on success, -1 on error. If the
   element does not exist, ignore and return 0. */
int set_remove(struct set *set, void *ele);

/* Return non-zero if the set contains the element, 0 otherwise. */
int set_contains(struct set *set, void *ele);

void set_clear(struct set *set);

struct set_iter {
  struct hashtbl_iter iter;
};

int set_iter_init(struct set_iter *iter, struct set *set);
void set_iter_inc(struct set_iter *iter);
void *set_iter_get(struct set_iter *iter);

#endif