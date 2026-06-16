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

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fs.h"
#include "obj/obj.h"
#include "ref.h"
#include "sha1.h"

#define MINI_SHA1_LENGTH 4

/* Resolve a short sha1 hash to a full sha1 hash, return 0 and write the result
   to sha1, return -1 if multiple matches. */
static int resolve_short_sha1(struct repo *repo, const char *name,
                              unsigned char *sha1)
{
  if (strlen(name) < MINI_SHA1_LENGTH) {
    errno = EINVAL;
    return -1;
  }
  char *normname = strdup(name);
  if (!normname)
    return -1;
  for (size_t i = 0; i < strlen(normname); i++) {
    if (!isxdigit(normname[i])) {
      errno = EINVAL;
      free(normname);
      return -1;
    }
    normname[i] = tolower(normname[i]);
  }

  char path[PATH_MAX];
  if (snprintf(path, sizeof(path), "%s/objects/%c%c", repo->gitdir, normname[0],
               normname[1]) >= PATH_MAX) {
    errno = ENAMETOOLONG;
    free(normname);
    return -1;
  }

  if (!dir_exists(path)) {
    errno = ENOENT;
    free(normname);
    return -1;
  }

  struct dirent *entry;
  DIR *dir = opendir(path);
  if (dir == NULL) {
    free(normname);
    return -1;
  }

  int match = 0;

  while ((entry = readdir(dir)) != NULL) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
      continue;
    if (strncmp(entry->d_name, &normname[2], strlen(&normname[2])) == 0) {
      if (match) {
        errno = EINVAL;
        closedir(dir);
        free(normname);
        return -1;
      }
      match = 1;
      unsigned char hex[SHA1_HEX_LENGTH];
      if (snprintf((char *)hex, SHA1_HEX_LENGTH, "%c%c%s", normname[0],
                   normname[1], entry->d_name) >= SHA1_HEX_LENGTH) {
        errno = ENAMETOOLONG;
        closedir(dir);
        free(normname);
        return -1;
      }
      if (hex_to_sha1(hex, sha1) == -1) {
        closedir(dir);
        free(normname);
        return -1;
      }
    }
  }

  closedir(dir);
  free(normname);
  if (!match) {
    errno = ENOENT;
    return -1;
  }
  return 0;
}

/* Resolve order:
    full sha1 hash (40 characters)
    HEAD reference
    full reference path : refs/heads/branch, refs/tags/tag
    branch name or tag name: branch, tag (can both exist but sha1 must be the
   same for both)
    short sha1 hash (minimum 4 characters)
*/
int obj_resolve(struct repo *repo, const char *name, unsigned char *sha1)
{
  if (!repo || !name || !sha1) {
    errno = EINVAL;
    return -1;
  }

  /* full sha1 hash */
  if (strlen(name) == SHA1_HEX_LENGTH - 1) {
    if (hex_to_sha1((unsigned char *)name, sha1) == -1)
      return -1;
    return 0;
  }

  /* HEAD reference */
  if (strcmp(name, "HEAD") == 0)
    return ref_read(repo, name, sha1);

  /* full reference path */
  if (strncmp(name, "refs/", 5) == 0)
    return ref_read(repo, name, sha1);

  char path[PATH_MAX];
  int found = 0;
  unsigned char temp[SHA1_DIGEST_LENGTH];

  /* branch name */
  if (snprintf(path, sizeof(path), "refs/heads/%s", name) >= PATH_MAX) {
    errno = ENAMETOOLONG;
    return -1;
  }
  if (!ref_read(repo, path, sha1))
    found = 1;

  /* tag name */
  if (snprintf(path, sizeof(path), "refs/tags/%s", name) >= PATH_MAX) {
    errno = ENAMETOOLONG;
    return -1;
  }
  if (!ref_read(repo, path, temp)) {
    if (found && memcmp(temp, sha1, SHA1_DIGEST_LENGTH) != 0) { /* ambiguous */
      errno = EINVAL;
      return -1;
    }
    if (!found)
      memcpy(sha1, temp, SHA1_DIGEST_LENGTH);
    found = 1;
  }

  if (found)
    return 0;

  /* short sha1 hash */
  if (!resolve_short_sha1(repo, name, sha1))
    return 0;

  return -1;
}