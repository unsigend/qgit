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

#include "tag.h"

#include <libqgit/object/commit.h>

static int parse_header(qgit_tag *out, char *start, char *end)
{
    char *p = start;
    size_t rem;
    int target_oid_parsed, type_parsed, tag_name_parsed, tagger_parsed;
    target_oid_parsed = type_parsed = tag_name_parsed = tagger_parsed = 0;

    while (p < end) {
        if (*p == '\n') /* skip \n */
        {
            p++;
            continue;
        }

        rem = end - p; /* remaining length of the line */

        if (rem >= 7 && strncmp(p, "object ", 7) == 0) /* target oid field */
        {
            if (target_oid_parsed) {
                qgit_seterror(QGITERR_BADTAGFILE);
                return -1;
            }
            target_oid_parsed = 1;
            p += 7;

            if (p + QGIT_OID_HEXSZ >= end) {
                qgit_seterror(QGITERR_BADTAGFILE);
                return -1;
            }
            if (qgit_oid_fromstr(&out->target_oid, p) < 0) {
                qgit_seterror(QGITERR_BADTAGFILE);
                return -1;
            }
            p += QGIT_OID_HEXSZ;

        } else if (rem >= 5 &&
                   strncmp(p, "type ", 5) == 0) /* target type field */
        {
            if (type_parsed) {
                qgit_seterror(QGITERR_BADTAGFILE);
                return -1;
            }
            type_parsed = 1;
            p += 5;

            if (p >= end) {
                qgit_seterror(QGITERR_BADTAGFILE);
                return -1;
            }

            char *nl = memchr(p, '\n', end - p);
            if (!nl) {
                qgit_seterror(QGITERR_BADTAGFILE);
                return -1;
            }
            *nl++ = '\0';

            qgit_obj_type type = qgit_object_string2type(p);
            if (type == QGIT_OBJ_BAD) {
                qgit_seterror(QGITERR_BADTAGFILE);
                return -1;
            }
            out->target_type = type;
            p = nl;
        } else if (rem >= 4 && strncmp(p, "tag ", 4) == 0) /* tag name field */
        {
            if (tag_name_parsed) {
                qgit_seterror(QGITERR_BADTAGFILE);
                return -1;
            }

            tag_name_parsed = 1;
            p += 4;

            if (p >= end) {
                qgit_seterror(QGITERR_BADTAGFILE);
                return -1;
            }

            const char *tagname = p;
            p = strchr(p, '\n');
            if (!p) {
                qgit_seterror(QGITERR_BADTAGFILE);
                return -1;
            }
            *p++ = '\0';

            out->tag_name = strdup(tagname);
            if (!out->tag_name)
                return -1;
        } else if (rem >= 7 && strncmp(p, "tagger ", 7) == 0) /* tagger field */
        {
            if (tagger_parsed) {
                qgit_seterror(QGITERR_BADTAGFILE);
                return -1;
            }
            tagger_parsed = 1;
            p += 7;

            if (p >= end) {
                qgit_seterror(QGITERR_BADTAGFILE);
                return -1;
            }

            if ((p = qgit_signature_parse(&out->tagger, p, end)) == NULL)
                return -1;

        } else {
            while (p < end && *p != '\n') /* skip unknown line */
                p++;
        }
    }

    if (!target_oid_parsed || !type_parsed || !tag_name_parsed ||
        !tagger_parsed) {
        qgit_seterror(QGITERR_BADTAGFILE);
        return -1;
    }

    return 0;
}

int tag_parse(qgit_tag *out, qgit_odb_object *odb_obj)
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

    if (parse_header(out, payload, header_end) < 0)
        return -1;

    return 0;
}
