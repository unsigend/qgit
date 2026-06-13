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

#include <string.h>

#include "collection/set.h"

int set_init(struct set *set, struct set_fns *fns)
{
  if (!set || !fns || !fns->hash || !fns->cmp)
    return -1;
  memset(set, 0, sizeof(struct set));
  set->tbl_fns.hash = fns->hash;
  set->tbl_fns.cmp = fns->cmp;
  set->tbl_fns.destroy_key = fns->destroy;
  set->tbl_fns.destroy_val = NULL;
  return hashtbl_init(&set->tbl, &set->tbl_fns);
}

void set_fini(struct set *set)
{
  if (!set)
    return;
  hashtbl_fini(&set->tbl);
  memset(set, 0, sizeof(struct set));
}

int set_insert(struct set *set, void *ele)
{
  if (!set || !ele)
    return -1;

  if (set_contains(set, ele))
    return 0;

  return hashtbl_insert(&set->tbl, ele, ele);
}

int set_remove(struct set *set, void *ele)
{
  if (!set || !ele)
    return -1;

  if (!set_contains(set, ele))
    return 0;

  return hashtbl_remove(&set->tbl, ele, NULL);
}

int set_contains(struct set *set, void *ele)
{
  if (!set || !ele)
    return 0;
  return hashtbl_findnode(&set->tbl, ele) != NULL;
}

void set_clear(struct set *set)
{
  if (!set)
    return;
  hashtbl_clear(&set->tbl);
}

int set_iter_init(struct set_iter *iter, struct set *set)
{
  if (!iter || !set)
    return -1;
  return hashtbl_iter_init(&iter->iter, &set->tbl);
}

void set_iter_inc(struct set_iter *iter)
{
  if (!iter)
    return;
  hashtbl_iter_inc(&iter->iter);
}

void *set_iter_get(struct set_iter *iter)
{
  if (!iter)
    return NULL;
  struct hashtbl_node *node = hashtbl_iter_get(&iter->iter);
  if (!node)
    return NULL;
  return node->key;
}