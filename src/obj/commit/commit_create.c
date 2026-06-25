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

#include <stdlib.h>

#include "collection/slist.h"
#include "obj/commit.h"
#include "obj/object.h"

static int fmt_commit(const struct commit *commit, void *buf, size_t buflen)
{
  if (!commit)
    return -1;
  if (!commit->author.name || !commit->author.email)
    return -1;

  int n = 0, sz = 0;
  struct slist_iter it;
  unsigned char hex[SHA1_HEXLEN];

  if (sha1_to_hex(commit->tree, hex) == -1)
    return -1;

  if ((n = snprintf(buf, buflen, "tree %s\n", hex)) < 0)
    return -1;
  sz += n;

  if (!slist_empty(&commit->parents)) {
    if (slist_iter_init(&it, (struct slist *)&commit->parents) == -1)
      return -1;

    while (slist_iter_get(&it)) {
      if (sha1_to_hex(slist_iter_get(&it), hex) == -1)
        return -1;
      if ((n = snprintf(buf ? buf + sz : NULL, buflen ? buflen - sz : 0,
                        "parent %s\n", hex)) < 0)
        return -1;
      sz += n;
      slist_iter_inc(&it);
    }
  }

  if ((n = snprintf(buf ? buf + sz : NULL, buflen ? buflen - sz : 0,
                    "author %s <%s> %ld %s\n", commit->author.name,
                    commit->author.email, commit->author.time,
                    commit->author.zone)) < 0)
    return -1;
  sz += n;

  if (commit->committer.name && commit->committer.email) {
    if ((n = snprintf(buf ? buf + sz : NULL, buflen ? buflen - sz : 0,
                      "committer %s <%s> %ld %s\n", commit->committer.name,
                      commit->committer.email, commit->committer.time,
                      commit->committer.zone)) < 0)
      return -1;
  } else if ((n = snprintf(buf ? buf + sz : NULL, buflen ? buflen - sz : 0,
                           "committer %s <%s> %ld %s\n", commit->author.name,
                           commit->author.email, commit->author.time,
                           commit->author.zone)) < 0)
    return -1;
  sz += n;

  if ((n = snprintf(buf ? buf + sz : NULL, buflen ? buflen - sz : 0, "\n")) < 0)
    return -1;
  sz += n;

  if (commit->msg && commit->msg[0]) {
    if ((n = snprintf(buf ? buf + sz : NULL, buflen ? buflen - sz : 0, "%s\n",
                      commit->msg)) < 0)
      return -1;
    sz += n;
  }

  if (buflen && sz >= (int)buflen)
    return -1;
  return sz;
}

struct object *commit_create(const struct commit *commit)
{
  if (!commit || !commit->author.name || !commit->author.email)
    return NULL;

  struct object *obj = NULL;
  int payloadsz = 0;
  void *payload = NULL;

  if ((payloadsz = fmt_commit(commit, NULL, 0)) < 0)
    return NULL;

  if (!(payload = malloc(payloadsz + 1)))
    return NULL;

  if (fmt_commit(commit, payload, payloadsz + 1) < 0) {
    free(payload);
    return NULL;
  }

  if (!(obj = obj_create(payload, payloadsz, OBJ_COMMIT))) {
    free(payload);
    return NULL;
  }

  free(payload);
  return obj;
}