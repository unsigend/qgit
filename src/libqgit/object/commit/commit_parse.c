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

#include <assert.h>
#include <collection/vector.h>
#include <libqgit/error.h>
#include <libqgit/object/commit.h>
#include <libqgit/oid.h>
#include <stddef.h>
#include <string.h>

static int parse_header(qgit_commit *out, char *start, char *end)
{
    char *p = start;
    size_t rem;
    int tree_parsed, author_parsed, committer_parsed;
    tree_parsed = author_parsed = committer_parsed = 0;

    while (p < end) {
        if (*p == '\n') /* skip \n */
        {
            p++;
            continue;
        }

        rem = end - p; /* remaining length of the line */

        if (rem >= 5 && strncmp(p, "tree ", 5) == 0) /* tree oid field */
        {
            if (tree_parsed) {
                qgit_seterror(QGITERR_BADCOMMITFILE);
                return -1;
            }
            tree_parsed = 1;
            p += 5;

            if (p + QGIT_OID_HEXSZ >= end) {
                qgit_seterror(QGITERR_BADCOMMITFILE);
                return -1;
            }
            if (qgit_oid_fromstr(&out->tree_oid, p) < 0) {
                qgit_seterror(QGITERR_BADCOMMITFILE);
                return -1;
            }
            p += QGIT_OID_HEXSZ;

        } else if (rem >= 7 &&
                   strncmp(p, "parent ", 7) == 0) /* parent oid field */
        {
            p += 7;
            qgit_oid parent_oid;

            if (p + QGIT_OID_HEXSZ >= end) {
                qgit_seterror(QGITERR_BADCOMMITFILE);
                return -1;
            }

            if (qgit_oid_fromstr(&parent_oid, p) < 0) {
                qgit_seterror(QGITERR_BADCOMMITFILE);
                return -1;
            }

            p += QGIT_OID_HEXSZ;

            if (vec_pushback(out->parents_oids, &parent_oid) < 0)
                return -1;

        } else if (rem >= 7 && strncmp(p, "author ", 7) == 0) /* author field */
        {
            if (author_parsed) {
                qgit_seterror(QGITERR_BADCOMMITFILE);
                return -1;
            }
            author_parsed = 1;
            p += 7;

            if (p >= end) {
                qgit_seterror(QGITERR_BADCOMMITFILE);
                return -1;
            }

            if ((p = qgit_signature_parse(&out->author, p, end)) == NULL)
                return -1;

        } else if (rem >= 10 &&
                   strncmp(p, "committer ", 10) == 0) /* committer field */
        {
            if (committer_parsed) {
                qgit_seterror(QGITERR_BADCOMMITFILE);
                return -1;
            }
            committer_parsed = 1;
            p += 10;

            if (p >= end) {
                qgit_seterror(QGITERR_BADCOMMITFILE);
                return -1;
            }

            if ((p = qgit_signature_parse(&out->committer, p, end)) == NULL)
                return -1;

        } else {
            while (p < end && *p != '\n') /* skip unknown line */
                p++;
        }
    }

    if (!tree_parsed || !author_parsed || !committer_parsed) {
        qgit_seterror(QGITERR_BADCOMMITFILE);
        return -1;
    }

    return 0;
}

int commit_parse(qgit_commit *out, qgit_odb_object *odb_obj)
{
    assert(out && odb_obj);

    char *payload = (char *)qgit_odb_object_data(odb_obj);
    char *end = payload + qgit_odb_object_size(odb_obj);
    char *body, *header_end;

    body = memmem(payload, end - payload, "\n\n", 2);

    if (body) {
        header_end = body + 1; /* points to the end of the header */
        out->message = strndup(body + 2, end - (body + 2));
        if (!out->message)
            return -1;
    } else {
        out->message = NULL;
        header_end = end;
    }

    if (vec_init(&out->parents_oids, sizeof(qgit_oid), NULL) < 0)
        return -1;

    if (parse_header(out, payload, header_end) < 0)
        return -1;

    return 0;
}
