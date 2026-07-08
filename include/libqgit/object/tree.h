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

#ifndef LIBQGIT_OBJECT_TREE_H
#define LIBQGIT_OBJECT_TREE_H

#include <libqgit/common.h>
#include <libqgit/object/object.h>
#include <libqgit/oid.h>
#include <libqgit/types.h>

QGIT_BEGIN_DECLS

/* Tree walk order. */
typedef enum {
    QGIT_TREEWALK_PRE = 0, /* pre-order: parent directory before its children */
    QGIT_TREEWALK_POST =
        1, /* post-order: children before their parent directory */
} qgit_treewalk_mode;

/**
 * Callback invoked once per entry during qgit_tree_walk.
 *
 * @param root  relative directory path of the entry (empty string for the root
 *              tree)
 * @param entry current entry
 * @param payload opaque pointer forwarded to each callback invocation
 * @return 0 to continue the traversal, negative value to skip descending into
 * a subtree, or positive value to stop the traversal on error
 */
typedef int (*qgit_treewalk_cb)(const char *root, qgit_tree_entry *entry,
                                void *payload);

/**
 * Look up a tree object in a repository by OID.
 *
 * @param out  output pointer to receive the tree handle, must not be NULL
 * @param repo repository to search, must not be NULL
 * @param id   OID of the tree, must not be NULL
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_INLINE(int)
qgit_tree_lookup(qgit_tree **out, qgit_repository *repo, const qgit_oid *id)
{
    return qgit_object_lookup((qgit_object **)out, repo, id, QGIT_OBJ_TREE);
}

/**
 * Look up a tree object in a repository by abbreviated OID prefix.
 *
 * @param out  output pointer to receive the tree handle, must not be NULL
 * @param repo repository to search, must not be NULL
 * @param id   partial OID with the unused suffix zeroed
 * @param len  number of hex characters in the prefix
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_INLINE(int)
qgit_tree_lookup_prefix(qgit_tree **out, qgit_repository *repo,
                        const qgit_oid *id, unsigned int len)
{
    return qgit_object_lookup_prefix((qgit_object **)out, repo, id, len,
                                     QGIT_OBJ_TREE);
}

/**
 * Free a tree handle.
 *
 * @param tree tree to free, no-op if NULL
 */
QGIT_INLINE(void) qgit_tree_free(qgit_tree *tree)
{
    qgit_object_free((qgit_object *)tree);
}

/**
 * Return the OID of a tree object.
 *
 * @param tree tree to query, must not be NULL
 * @return pointer to the tree OID, owned by the tree
 */
QGIT_EXTERN(const qgit_oid *) qgit_tree_id(qgit_tree *tree);

/**
 * Return the number of direct entries in a tree.
 *
 * @param tree tree to query, must not be NULL
 * @return number of entries
 */
QGIT_EXTERN(unsigned int) qgit_tree_entrycount(qgit_tree *tree);

/**
 * Look up a direct tree entry by filename.
 *
 * @param tree     tree to search, must not be NULL
 * @param filename name of the entry to find
 * @return pointer to the entry, or NULL if not found
 */
QGIT_EXTERN(const qgit_tree_entry *)
qgit_tree_entry_byname(qgit_tree *tree, const char *filename);

/**
 * Look up a direct tree entry by its position index.
 *
 * @param tree tree to query, must not be NULL
 * @param idx  zero-based index into the entry list
 * @return pointer to the entry, or NULL if idx is out of range
 */
QGIT_EXTERN(const qgit_tree_entry *)
qgit_tree_entry_byindex(qgit_tree *tree, unsigned int idx);

/**
 * Return the file mode attributes of a tree entry.
 *
 * @param entry tree entry to query, must not be NULL
 * @return UNIX file mode (e.g. 0100644, 0040000)
 */
QGIT_EXTERN(unsigned int)
qgit_tree_entry_attributes(const qgit_tree_entry *entry);

/**
 * Return the filename of a tree entry.
 *
 * @param entry tree entry to query, must not be NULL
 * @return null-terminated filename, owned by the entry
 */
QGIT_EXTERN(const char *) qgit_tree_entry_name(const qgit_tree_entry *entry);

/**
 * Return the OID of the object a tree entry points to.
 *
 * @param entry tree entry to query, must not be NULL
 * @return pointer to the OID, owned by the entry
 */
QGIT_EXTERN(const qgit_oid *) qgit_tree_entry_id(const qgit_tree_entry *entry);

/**
 * Return the object type a tree entry points to.
 *
 * @param entry tree entry to query, must not be NULL
 * @return QGIT_OBJ_BLOB or QGIT_OBJ_TREE
 */
QGIT_EXTERN(qgit_obj_type) qgit_tree_entry_type(const qgit_tree_entry *entry);

/**
 * Load the object a tree entry points to.
 *
 * The returned object must be freed with qgit_object_free.
 *
 * @param out   output pointer to receive the loaded object, must not be NULL
 * @param repo  repository to load from, must not be NULL
 * @param entry tree entry to follow, must not be NULL
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int)
qgit_tree_entry_to_object(qgit_object **out, qgit_repository *repo,
                          const qgit_tree_entry *entry);

/**
 * Retrieve a subtree at a relative path below a root tree.
 *
 * The returned tree must be freed with qgit_tree_free.
 *
 * @param out          output pointer to receive the subtree, must not be NULL
 * @param root         tree to start from, must not be NULL
 * @param subtree_path slash-separated path to the subtree (e.g. "src/lib")
 * @return 0 on success, -1 if the path does not exist or on error and sets
 * errno
 */
QGIT_EXTERN(int)
qgit_tree_get_subtree(qgit_tree **out, qgit_tree *root,
                      const char *subtree_path);

/**
 * Walk a tree and all of its subtrees in pre- or post-order.
 *
 * The callback is invoked once per entry. For subtrees, returning a
 * negative value skips descending into that subtree. Returning any other
 * non-zero value stops the traversal.
 *
 * @param tree     tree to walk, must not be NULL
 * @param callback function called for each entry, must not be NULL
 * @param mode     QGIT_TREEWALK_PRE or QGIT_TREEWALK_POST
 * @param payload  opaque pointer forwarded to each callback invocation
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int)
qgit_tree_walk(qgit_tree *tree, qgit_treewalk_cb callback,
               qgit_treewalk_mode mode, void *payload);

/**
 * Write a tree to the ODB from the current state of the index.
 *
 * Recursively creates tree objects for every subdirectory in the index
 * and returns the OID of the root tree.
 *
 * @param oid   output pointer to receive the root tree OID, must not be NULL
 * @param index index to build the tree from, must not be NULL
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int) qgit_tree_create_fromindex(qgit_oid *oid, qgit_index *index);

/**
 * Allocate a new in-memory tree builder.
 *
 * If source is non-NULL the builder is pre-populated with the entries
 * of that tree, otherwise it starts empty.
 *
 * @param out    output pointer to receive the builder, must not be NULL
 * @param source optional tree to copy entries from, or NULL for empty
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int)
qgit_treebuilder_create(qgit_treebuilder **out, const qgit_tree *source);

/**
 * Remove all entries from a tree builder without freeing it.
 *
 * @param builder tree builder to clear, must not be NULL
 */
QGIT_EXTERN(void) qgit_treebuilder_clear(qgit_treebuilder *builder);

/**
 * Free a tree builder and all its entries.
 *
 * @param builder tree builder to free, no-op if NULL
 */
QGIT_EXTERN(void) qgit_treebuilder_free(qgit_treebuilder *builder);

/**
 * Look up an entry in the builder by filename.
 *
 * The returned pointer is owned by the builder and is valid until the
 * entry is removed or the builder is freed.
 *
 * @param builder  tree builder to search, must not be NULL
 * @param filename name of the entry to find
 * @return pointer to the entry, or NULL if not found
 */
QGIT_EXTERN(const qgit_tree_entry *)
qgit_treebuilder_get(qgit_treebuilder *builder, const char *filename);

/**
 * Insert or update an entry in the builder.
 *
 * If an entry named filename already exists its OID and attributes are
 * updated. The optional entry_out receives a pointer to the inserted entry,
 * owned by the builder.
 *
 * @param entry_out optional output pointer to receive the entry, or NULL
 * @param builder    tree builder to modify, must not be NULL
 * @param filename   name of the entry, must not be NULL
 * @param id         OID of the object the entry points to, must not be NULL
 * @param attributes UNIX file mode for the entry (e.g. 0100644, 0040000)
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int)
qgit_treebuilder_insert(qgit_tree_entry **entry_out, qgit_treebuilder *builder,
                        const char *filename, const qgit_oid *id,
                        unsigned int attributes);

/**
 * Remove an entry from the builder by filename.
 *
 * @param builder  tree builder to modify, must not be NULL
 * @param filename name of the entry to remove
 * @return 0 on success, -1 if not found or on error and sets errno
 */
QGIT_EXTERN(int)
qgit_treebuilder_remove(qgit_treebuilder *builder, const char *filename);

/**
 * Write the builder contents to the ODB as a tree object.
 *
 * @param oid  output pointer to receive the written tree OID, must not be NULL
 * @param repo repository to write into, must not be NULL
 * @param builder  tree builder to write, must not be NULL
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int)
qgit_treebuilder_write(qgit_oid *oid, qgit_repository *repo,
                       qgit_treebuilder *builder);

QGIT_END_DECLS

#endif
