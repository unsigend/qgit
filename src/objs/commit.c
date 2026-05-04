/* qgit - A simplified git like version control system
 * Copyright (C) 2025 - 2026 Qiu Yixiang
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

#include "objs/commit.h"
#include "objs/repo.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Parse a key value pair from the raw data. Return next line raw pointer.*/
static char *parsekv(char *raw, char **key, char **val)
{
  if (!raw || !key || !val)
    return NULL;

  *key = raw;
  char *s = strchr(raw, ' ');
  if (!s)
    return NULL;
  *s++ = '\0';
  *val = s;

  while (*s && *s != '\n')
    s++;

  *s = '\0';
  return s + 1;
}

struct commit *commit_parse(struct object *obj)
{
  if (!obj)
    return NULL;

  struct commit *commit = malloc(sizeof(struct commit));
  if (!commit)
    return NULL;
  memset(commit, 0, sizeof(struct commit));

  char *key = NULL, *val = NULL;
  char *raw = (char *)obj->payload;

  while (*raw != '\n') {
    char *line = parsekv(raw, &key, &val);
    if (!line)
      break;
    if (strcmp(key, "tree") == 0) {
      strcpy(commit->tree, val);
      commit->tree[40] = '\0';
    } else if (strcmp(key, "parent") == 0) {
      void *newparents =
          realloc(commit->parents, sizeof(char *) * (commit->nparents + 1));
      if (!newparents) {
        commit_free(commit);
        return NULL;
      }
      commit->parents = newparents;
      commit->parents[commit->nparents] = strdup(val);
      if (!commit->parents[commit->nparents]) {
        commit_free(commit);
        return NULL;
      }
      commit->nparents++;
    } else if (strcmp(key, "author") == 0) {
      commit->author = strdup(val);
      if (!commit->author) {
        commit_free(commit);
        return NULL;
      }
    } else if (strcmp(key, "committer") == 0) {
      commit->committer = strdup(val);
      if (!commit->committer) {
        commit_free(commit);
        return NULL;
      }
    }
    raw = line;
  }

  if (*raw == '\n')
    raw++;

  commit->message = strndup(raw, obj->size - (raw - (char *)obj->payload));
  if (!commit->message) {
    commit_free(commit);
    return NULL;
  }

  return commit;
}

void commit_free(struct commit *commit)
{
  if (!commit)
    return;
  if (commit->parents) {
    for (int i = 0; i < commit->nparents; i++)
      free(commit->parents[i]);
    free(commit->parents);
  }
  if (commit->author)
    free(commit->author);
  if (commit->committer)
    free(commit->committer);
  if (commit->message)
    free(commit->message);
  free(commit);
}