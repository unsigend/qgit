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

#include <libqgit/repo/refs.h>

static void tag_name_free(void *data)
{
    if (!data)
        return;
    char *name = *(char **)data;
    free(name);
}

static int tag_name_cmp(const void *a, const void *b)
{
    const char *name1 = *(const char **)a;
    const char *name2 = *(const char **)b;
    return strcmp(name1, name2);
}

int qgit_tag_list(struct vector **out, qgit_repository *repo)
{
    assert(out && repo);
    *out = NULL;

    struct vector *refs;
    struct vector *tags;

    if (qgit_reference_list(&refs, repo, QGIT_REF_LIST_TAGS) < 0)
        return -1;

    if (vec_init(&tags, sizeof(char *), tag_name_free) < 0) {
        vec_free(refs);
        return -1;
    }

    for (size_t i = 0; i < vec_size(refs); i++) {
        const char *name = *(char **)vec_at(refs, i);
        char *tagname;

        tagname = strdup(name + 10); /* skip "refs/tags/" prefix */
        if (!tagname) {
            vec_free(refs);
            vec_free(tags);
            return -1;
        }

        if (vec_pushback(tags, &tagname) < 0) {
            free(tagname);
            vec_free(refs);
            vec_free(tags);
            return -1;
        }
    }

    vec_sort(tags, tag_name_cmp);
    *out = tags;

    vec_free(refs);

    return 0;
}
