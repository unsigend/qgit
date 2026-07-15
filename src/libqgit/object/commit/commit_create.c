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
#include "commit.h"

#include <errno.h>
#include <libqgit/object/commit.h>
#include <libqgit/repo/refs.h>
#include <libqgit/repo/repository.h>
#include <stdio.h>
#include <string.h>

static int commit_fmt(char *buf, size_t buflen, const qgit_signature *author,
                      const qgit_signature *committer, const char *message,
                      const qgit_tree *tree, int parent_count,
                      const qgit_commit *parents[])
{
    int total = 0, nwrite;

    if (buf && buflen) {
        char hex[QGIT_OID_HEXSZ + 1];
        char *p = buf;
        size_t message_len = 0;

        qgit_oid_fmt(hex, qgit_tree_id((qgit_tree *)tree));
        hex[QGIT_OID_HEXSZ] = '\0';

        if ((nwrite = snprintf(p, buflen - total, "tree %s\n", hex)) < 0 ||
            (size_t)nwrite >= buflen - total) /* tree */
            return -1;
        total += nwrite;
        p += nwrite;

        for (int i = 0; i < parent_count; i++) /* parents */
        {
            qgit_oid_fmt(hex, qgit_commit_id((qgit_commit *)parents[i]));
            hex[QGIT_OID_HEXSZ] = '\0';
            if ((nwrite = snprintf(p, buflen - total, "parent %s\n", hex)) <
                    0 ||
                (size_t)nwrite >= buflen - total)
                return -1;
            total += nwrite;
            p += nwrite;
        }

        if ((nwrite =
                 snprintf(p, buflen - total, "author %s <%s> %ld %c%.2d%.2d\n",
                          author->name, author->email, author->when.time,
                          author->when.offset < 0 ? '-' : '+',
                          abs(author->when.offset / 60),
                          abs(author->when.offset % 60))) < 0 ||
            (size_t)nwrite >= buflen - total) /* author */
            return -1;
        total += nwrite;
        p += nwrite;

        if ((nwrite = snprintf(
                 p, buflen - total, "committer %s <%s> %ld %c%.2d%.2d\n",
                 committer->name, committer->email, committer->when.time,
                 committer->when.offset < 0 ? '-' : '+',
                 abs(committer->when.offset / 60),
                 abs(committer->when.offset % 60))) < 0 ||
            (size_t)nwrite >= buflen - total) /* committer */
            return -1;
        total += nwrite;
        p += nwrite;

        if (total + 1 > (int)buflen)
            return -1;
        *p++ = '\n';
        total++;

        if (message) {
            message_len = strlen(message);

            if (total + message_len + 1 > buflen)
                return -1;
            strcpy(p, message);
            p += message_len;
            total += message_len;
        }

        if (total + 1 > (int)buflen)
            return -1;

        *p++ = '\n';
        total++;

        return total;

    } else /* dry run */
    {
        total += QGIT_OID_HEXSZ + strlen("tree") + 2; /* tree */
        total += parent_count *
                 (QGIT_OID_HEXSZ + strlen("parent") + 2); /* parents */
        if ((nwrite = snprintf(NULL, 0, "author %s <%s> %ld %c%.2d%.2d\n",
                               author->name, author->email, author->when.time,
                               author->when.offset < 0 ? '-' : '+',
                               abs(author->when.offset / 60),
                               abs(author->when.offset % 60))) < 0)
            return -1;
        total += nwrite;
        if ((nwrite = snprintf(NULL, 0, "committer %s <%s> %ld %c%.2d%.2d\n",
                               committer->name, committer->email,
                               committer->when.time,
                               committer->when.offset < 0 ? '-' : '+',
                               abs(committer->when.offset / 60),
                               abs(committer->when.offset % 60))) < 0)
            return -1;
        total += nwrite;

        total += (message ? strlen(message) : 0) + 2; /* message */
        return total;
    }
}

int qgit_commit_create(qgit_oid *oid, qgit_repository *repo,
                       const char *update_ref, const qgit_signature *author,
                       const qgit_signature *committer, const char *message,
                       const qgit_tree *tree, int parent_count,
                       const qgit_commit *parents[])
{
    int total;
    char *buf;
    size_t buflen;
    qgit_reference *ref;

    if ((total = commit_fmt(NULL, 0, author, committer, message, tree,
                            parent_count, parents)) < 0)
        return -1;
    buflen = (size_t)total;

    if ((buf = malloc(buflen)) == NULL)
        return -1;

    if (commit_fmt(buf, buflen, author, committer, message, tree, parent_count,
                   parents) < 0) {
        free(buf);
        return -1;
    }

    if (qgit_odb_write(oid, qgit_repository_odb(repo), buf, buflen,
                       QGIT_OBJ_COMMIT) < 0) {
        free(buf);
        return -1;
    }

    free(buf);

    if (update_ref) {
        errno = 0;
        if (qgit_reference_lookup(&ref, repo, update_ref) < 0) {
            if (errno == ENOENT) /* create new reference */
            {
                qgit_reference *new_ref;
                if (qgit_reference_create_oid(&new_ref, repo, update_ref, oid,
                                              0) < 0)
                    return -1;

                qgit_reference_free(new_ref);
                return 0;
            }
            return -1;
        }

        if (qgit_reference_type(ref) ==
            QGIT_REF_SYMBOLIC) /* resolve symbolic reference and update it */
        {
            qgit_reference *resolved;
            errno = 0;
            if (qgit_reference_resolve(&resolved, ref) < 0) {
                if (errno == ENOENT) /* create new reference */
                {
                    qgit_reference *new_ref;
                    if (qgit_reference_create_oid(&new_ref, repo,
                                                  qgit_reference_target(ref),
                                                  oid, 0) < 0) {
                        qgit_reference_free(ref);
                        return -1;
                    }
                    qgit_reference_free(ref);
                    qgit_reference_free(new_ref);
                    return 0;
                }
                qgit_reference_free(ref);
                return -1;
            }
            qgit_reference_free(ref);
            ref = resolved;
        }

        if (qgit_reference_set_oid(ref, oid) < 0) {
            qgit_reference_free(ref);
            return -1;
        }

        qgit_reference_free(ref);
    }
    return 0;
}
