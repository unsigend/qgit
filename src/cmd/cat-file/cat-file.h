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

#ifndef CMD_CAT_FILE_H
#define CMD_CAT_FILE_H

#include <argparse.h>
#include <die.h>
#include <libqgit/object/blob.h>
#include <libqgit/object/commit.h>
#include <libqgit/object/object.h>
#include <libqgit/object/tag.h>
#include <libqgit/object/tree.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct cmd_cat_file_flags {
    int pretty;     /* -p pretty-print object contents */
    int print_type; /* -t print object type */
    int print_size; /* -s print object size in bytes */
};

static struct cmd_cat_file_flags flags = {
    .pretty = 0,
    .print_type = 0,
    .print_size = 0,
};

static struct argparse_opt options[] = {
    OPT_HELP(),
    OPT_BOOL('p', NULL, "Pretty-print the object contents", &flags.pretty),
    OPT_BOOL('t', NULL, "Print the object type", &flags.print_type),
    OPT_BOOL('s', NULL, "Print the object size in bytes", &flags.print_size),
    OPT_END(),
};

static const char *usages[] = {
    "qgit cat-file (-p | -t | -s) <object>",
    "qgit cat-file <type> <object>",
};

static struct argparse_desc desc = {
    .prog = "qgit cat-file",
    .desc = "Provide contents or details of repository objects",
    .usages = usages,
    .nusages = sizeof(usages) / sizeof(usages[0]),
    .epilog = "Only one of -p, -t, or -s may be used. Supported types: "
              "blob, commit, tree, tag.",
};

static void pretty_print_commit(qgit_commit *commit)
{
    char hex[QGIT_OID_HEXSZ + 1];
    const qgit_signature *signature;
    const char *message = qgit_commit_message(commit);

    qgit_oid_fmt(hex, qgit_commit_tree_oid(commit));
    hex[QGIT_OID_HEXSZ] = '\0';
    fprintf(stdout, "tree %s\n", hex);

    for (size_t i = 0; i < qgit_commit_parentcount(commit); i++) {
        qgit_oid_fmt(hex, qgit_commit_parent_oid(commit, i));
        hex[QGIT_OID_HEXSZ] = '\0';
        fprintf(stdout, "parent %s\n", hex);
    }

    signature = qgit_commit_author(commit);
    if (signature) {
        fprintf(stdout, "author %s <%s> %ld %c%.2d%.2d\n", signature->name,
                signature->email, signature->when.time,
                (signature->when.offset >= 0 ? '+' : '-'),
                abs(signature->when.offset / 60),
                abs(signature->when.offset % 60));
    }

    signature = qgit_commit_committer(commit);
    if (signature) {
        fprintf(stdout, "committer %s <%s> %ld %c%.2d%.2d\n", signature->name,
                signature->email, signature->when.time,
                (signature->when.offset >= 0 ? '+' : '-'),
                abs(signature->when.offset / 60),
                abs(signature->when.offset % 60));
    }

    fprintf(stdout, "\n%s", message ? message : "");
}

static void pretty_print_tree(qgit_tree *tree)
{
    const qgit_tree_entry *entry;
    char hex[QGIT_OID_HEXSZ + 1];

    for (size_t i = 0; i < qgit_tree_entrycount(tree); i++) {
        entry = qgit_tree_entry_byindex(tree, i);
        qgit_oid_fmt(hex, qgit_tree_entry_id(entry));
        hex[QGIT_OID_HEXSZ] = '\0';
        fprintf(stdout, "%.6o %s %s\t%s\n", qgit_tree_entry_attributes(entry),
                qgit_tree_entry_type(entry) == QGIT_OBJ_TREE ? "tree" : "blob",
                hex, qgit_tree_entry_name(entry));
    }
}

static void pretty_print_blob(qgit_blob *blob)
{
    fwrite(qgit_blob_rawcontent(blob), 1, qgit_blob_rawsize(blob), stdout);
}

static void pretty_print_tag(qgit_tag *tag)
{
    char hex[QGIT_OID_HEXSZ + 1];
    qgit_obj_type type;
    const qgit_signature *signature;
    const char *message = qgit_tag_message(tag);

    qgit_oid_fmt(hex, qgit_tag_target_oid(tag));
    hex[QGIT_OID_HEXSZ] = '\0';
    fprintf(stdout, "object %s\n", hex);

    type = qgit_tag_type(tag);
    fprintf(stdout, "type %s\n", qgit_object_type2string(type));

    fprintf(stdout, "tag %s\n", qgit_tag_name(tag));
    signature = qgit_tag_tagger(tag);
    if (signature) {
        fprintf(stdout, "tagger %s <%s> %ld %c%.2d%.2d\n", signature->name,
                signature->email, signature->when.time,
                (signature->when.offset >= 0 ? '+' : '-'),
                abs(signature->when.offset / 60),
                abs(signature->when.offset % 60));
    }

    fprintf(stdout, "\n%s", message ? message : "");
}

static void pretty_print_object(qgit_object *object)
{
    switch (qgit_object_type(object)) {
    case QGIT_OBJ_COMMIT:
        pretty_print_commit((qgit_commit *)object);
        break;
    case QGIT_OBJ_TREE:
        pretty_print_tree((qgit_tree *)object);
        break;
    case QGIT_OBJ_BLOB:
        pretty_print_blob((qgit_blob *)object);
        break;
    case QGIT_OBJ_TAG:
        pretty_print_tag((qgit_tag *)object);
        break;
    default:
        die("unknown object type");
    }
}

#endif
