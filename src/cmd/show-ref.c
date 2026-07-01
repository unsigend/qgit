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

#include <argparse.h>
#include <collection/vector.h>
#include <die.h>
#include <libqgit.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SHOW_BRANCH 1
#define SHOW_TAG 2

struct payload {
    int show_type;
    struct vector *vec;
};

static void free_str(void *p) { free(*(char **)p); }
static int reference_cmp(const void *a, const void *b)
{
    return strcmp(*(const char **)a, *(const char **)b);
}

static int reference_foreach(const char *name, void *payload)
{
    struct payload *p = (struct payload *)payload;
    char *dupname;

    if (p->show_type == SHOW_BRANCH && strncmp(name, "refs/heads/", 11) != 0)
        return 0;
    if (p->show_type == SHOW_TAG && strncmp(name, "refs/tags/", 10) != 0)
        return 0;

    dupname = strdup(name);
    if (!dupname)
        return -1;
    if (vec_pushback(p->vec, &dupname) == -1) {
        free(dupname);
        return -1;
    }
    return 0;
}

static void show_head(qgit_repository *repository)
{
    qgit_reference *head_ref, *resolved_head_ref;
    char hex[QGIT_OID_HEXSZ];
    if (qgit_repository_head(&head_ref, repository) == -1)
        die_errno();
    if (qgit_reference_resolve(&resolved_head_ref, head_ref) == -1)
        die_errno();
    qgit_oid_fmt(hex, qgit_reference_oid(resolved_head_ref));
    fprintf(stdout, "%s HEAD\n", hex);
    qgit_reference_free(head_ref);
    qgit_reference_free(resolved_head_ref);
}

static void show_references(qgit_repository *repository, int show_type)
{
    struct vector vec;
    struct vector_iter iter;
    struct payload payload = {
        .show_type = show_type,
        .vec = &vec,
    };

    if (vec_init(&vec, sizeof(char *), free_str) == -1)
        die_errno();
    if (qgit_reference_foreach(repository, reference_foreach, &payload) == -1)
        die_errno();
    vec_sort(&vec, reference_cmp);

    if (vec_iter_init(&iter, &vec) == -1)
        die_errno();

    while (vec_iter_get(&iter)) {
        const char *name = *(const char **)vec_iter_get(&iter);
        char hex[QGIT_OID_HEXSZ];
        qgit_reference *ref;
        if (qgit_reference_lookup(&ref, repository, name) == -1)
            die_errno();
        qgit_oid_fmt(hex, qgit_reference_oid(ref));
        fprintf(stdout, "%s %s\n", hex, name);
        qgit_reference_free(ref);
        vec_iter_inc(&iter);
    }

    vec_fini(&vec);
}

int cmd_show_ref(int argc, char **argv)
{
    int head = 0;
    int branches = 0;
    int tags = 0;
    struct argparse parser;
    struct argparse_opt opts[] = {
        OPT_HELP(),
        OPT_BOOL(0, "head", "Show the HEAD reference", &head),
        OPT_BOOL(0, "branches", "Show all branches", &branches),
        OPT_BOOL(0, "tags", "Show all tags", &tags),
        OPT_END(),
    };

    static const char *usages[] = {
        "qgit show-ref [--head] [--branches] [--tags]",
    };
    struct argparse_desc desc = {
        .prog = "qgit show-ref",
        .desc = "List references in a local repository",
        .usages = usages,
        .nusages = sizeof(usages) / sizeof(usages[0]),
    };

    if (argparse_init(&parser, opts, &desc) == -1)
        die("%s", argparse_strerror(&parser));
    if (argparse_parse(&parser, argc, argv) == -1)
        die("%s", argparse_strerror(&parser));

    int show_branches = branches || (!branches && !tags);
    int show_tags = tags || (!branches && !tags);
    qgit_repository *repository;
    char repo_path[PATH_MAX];

    if (qgit_repository_discover(repo_path, PATH_MAX, ".") == -1)
        die_errno();
    if (qgit_repository_open(&repository, repo_path) == -1)
        die_errno();

    if (head)
        show_head(repository);
    if (show_branches)
        show_references(repository, SHOW_BRANCH);
    if (show_tags)
        show_references(repository, SHOW_TAG);

    qgit_repository_free(repository);
    argparse_fini(&parser);
    return 0;
}
