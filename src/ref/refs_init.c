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

#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "collection/vector.h"
#include "ref.h"
#include "repo.h"

enum scope {
  BRANCHES,
  TAGS,
};

/* path for the ref dir like refs/heads or refs/tags */
static int foreach (struct refs *refs, const char *path, enum scope scope)
{
  char buf[PATH_MAX];
  DIR *dir = NULL;
  struct dirent *ent = NULL;
  struct stat st;
  struct ref ref;

  if (snprintf(buf, PATH_MAX, "%s/%s", refs->repo->qgitdir, path) >= PATH_MAX) {
    errno = ENAMETOOLONG;
    return -1;
  }

  if (!(dir = opendir(buf))) {
    if (errno == ENOENT) /* skip when dir not exists */
      return 0;
    return -1;
  }

  while ((ent = readdir(dir)) != NULL) {
    if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
      continue;

    if (snprintf(buf, PATH_MAX, "%s/%s/%s", refs->repo->qgitdir, path,
                 ent->d_name) >= PATH_MAX) { /* full path */
      errno = ENAMETOOLONG;
      closedir(dir);
      return -1;
    }

    if (stat(buf, &st) == -1) {
      closedir(dir);
      return -1;
    }

    if (snprintf(buf, PATH_MAX, "%s/%s", path, ent->d_name) >=
        PATH_MAX) { /* path/name */
      errno = ENAMETOOLONG;
      closedir(dir);
      return -1;
    }

    if (S_ISDIR(st.st_mode)) /* recursive for dir */
    {
      if (foreach (refs, buf, scope) == -1) {
        closedir(dir);
        return -1;
      }

    } else if (!S_ISREG(st.st_mode)) /* not a regular file */
      continue;
    else /* regular ref */
    {
      ref.name = strdup(buf);
      if (!ref.name) {
        closedir(dir);
        return -1;
      }

      if (ref_resolve_path(refs->repo, buf, ref.sha1) == -1) {
        free(ref.name);
        closedir(dir);
        return -1;
      }

      if (scope == BRANCHES) /* branches */
      {
        if (vec_pushback(&refs->branches, &ref) == -1) {
          free(ref.name);
          closedir(dir);
          return -1;
        }
      } else /* tags */
      {
        if (vec_pushback(&refs->tags, &ref) == -1) {
          free(ref.name);
          closedir(dir);
          return -1;
        }
      }
    }
  }

  closedir(dir);
  return 0;
}

static void ref_destroy(void *ele)
{
  struct ref *ref = (struct ref *)ele;
  free(ref->name);
}

int refs_init(struct refs *refs, struct repo *repo)
{
  if (!refs || !repo)
    return -1;

  if (vec_init(&refs->branches, sizeof(struct ref), ref_destroy) == -1)
    return -1;
  if (vec_init(&refs->tags, sizeof(struct ref), ref_destroy) == -1) {
    vec_fini(&refs->branches);
    return -1;
  }

  refs->repo = repo;

  if (foreach (refs, "refs/heads", BRANCHES) == -1) {
    refs_fini(refs);
    return -1;
  }
  if (foreach (refs, "refs/tags", TAGS) == -1) {
    refs_fini(refs);
    return -1;
  }

  return 0;
}