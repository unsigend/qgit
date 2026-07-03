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

#ifndef LIBQGIT_OBJECT_TAG_H
#define LIBQGIT_OBJECT_TAG_H

#include <collection/vector.h>
#include <libqgit/common.h>
#include <libqgit/object/object.h>
#include <libqgit/object/signature.h>
#include <libqgit/oid.h>
#include <libqgit/types.h>

QGIT_BEGIN_DECLS

/**
 * Look up an annotated tag object in a repository by OID.
 *
 * @param out  output pointer to receive the tag handle, must not be NULL
 * @param repo repository to search, must not be NULL
 * @param id   OID of the tag object, must not be NULL
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_INLINE(int)
qgit_tag_lookup(qgit_tag **out, qgit_repository *repo, const qgit_oid *id)
{
    return qgit_object_lookup((qgit_object **)out, repo, id, QGIT_OBJ_TAG);
}

/**
 * Look up an annotated tag object in a repository by abbreviated OID prefix.
 *
 * @param out  output pointer to receive the tag handle, must not be NULL
 * @param repo repository to search, must not be NULL
 * @param id   partial OID with the unused suffix zeroed
 * @param len  number of hex characters in the prefix
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_INLINE(int)
qgit_tag_lookup_prefix(qgit_tag **out, qgit_repository *repo,
                       const qgit_oid *id, unsigned int len)
{
    return qgit_object_lookup_prefix((qgit_object **)out, repo, id, len,
                                     QGIT_OBJ_TAG);
}

/**
 * Free an annotated tag handle.
 *
 * @param tag tag to free, no-op if NULL
 */
QGIT_INLINE(void) qgit_tag_free(qgit_tag *tag)
{
    qgit_object_free((qgit_object *)tag);
}

/**
 * Return the OID of an annotated tag object itself.
 *
 * @param tag tag to query, must not be NULL
 * @return pointer to the tag OID, owned by the tag
 */
QGIT_EXTERN(const qgit_oid *) qgit_tag_id(qgit_tag *tag);

/**
 * Load the object an annotated tag points to.
 *
 * The returned object must be freed with qgit_object_free.
 *
 * @param out output pointer to receive the tagged object, must not be NULL
 * @param tag tag to dereference, must not be NULL
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int) qgit_tag_target(qgit_object **out, qgit_tag *tag);

/**
 * Return the OID of the object an annotated tag points to without loading it.
 *
 * @param tag tag to query, must not be NULL
 * @return pointer to the target OID, owned by the tag
 */
QGIT_EXTERN(const qgit_oid *) qgit_tag_target_oid(qgit_tag *tag);

/**
 * Return the type of the object an annotated tag points to.
 *
 * @param tag tag to query, must not be NULL
 * @return object type of the tagged object
 */
QGIT_EXTERN(qgit_obj_type) qgit_tag_type(qgit_tag *tag);

/**
 * Return the name of an annotated tag (without the refs/tags/ prefix).
 *
 * The returned pointer is owned by the tag and is valid until
 * qgit_tag_free is called.
 *
 * @param tag tag to query, must not be NULL
 * @return null-terminated tag name
 */
QGIT_EXTERN(const char *) qgit_tag_name(qgit_tag *tag);

/**
 * Return the tagger signature of an annotated tag.
 *
 * The returned pointer is owned by the tag and is valid until
 * qgit_tag_free is called.
 *
 * @param tag tag to query, must not be NULL
 * @return pointer to the tagger signature
 */
QGIT_EXTERN(const qgit_signature *) qgit_tag_tagger(qgit_tag *tag);

/**
 * Return the message of an annotated tag.
 *
 * The returned pointer is owned by the tag and is valid until
 * qgit_tag_free is called.
 *
 * @param tag tag to query, must not be NULL
 * @return null-terminated tag message
 */
QGIT_EXTERN(const char *) qgit_tag_message(qgit_tag *tag);

/**
 * Peel an annotated tag recursively until a non-tag object is reached.
 *
 * Used for the ^{} and ^{type} peel suffixes in revision syntax.
 * The returned object must be freed with qgit_object_free.
 *
 * @param out output pointer to receive the peeled object, must not be NULL
 * @param tag tag to peel, must not be NULL
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int) qgit_tag_peel(qgit_object **out, qgit_tag *tag);

/**
 * Create a new annotated tag object and write a reference under refs/tags/.
 *
 * qgit requires annotated tags to point at a commit. The tag object is
 * written to the ODB and refs/tags/<tag_name> is created (or replaced
 * when force is non-zero).
 *
 * @param oid      output pointer to receive the new tag OID, must not be NULL
 * @param repo     repository to write into, must not be NULL
 * @param tag_name short tag name (e.g. "v1.0"), must not be NULL
 * @param target   OID of the commit to tag, must not be NULL
 * @param tagger   tagger signature, must not be NULL
 * @param message  tag message, must not be NULL
 * @param force    if non-zero, overwrite an existing tag with the same name
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int)
qgit_tag_create(qgit_oid *oid, qgit_repository *repo, const char *tag_name,
                const qgit_oid *target, const qgit_signature *tagger,
                const char *message, int force);

/**
 * Create a new lightweight tag as a direct reference under refs/tags/.
 *
 * No tag object is written to the ODB. refs/tags/<tag_name> is created
 * pointing directly at the target OID.
 *
 * @param oid      output pointer to receive the target OID, must not be NULL
 * @param repo     repository to write into, must not be NULL
 * @param tag_name short tag name (e.g. "v1.0"), must not be NULL
 * @param target   OID of the object the tag will point to, must not be NULL
 * @param force    if non-zero, overwrite an existing tag with the same name
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int)
qgit_tag_create_lightweight(qgit_oid *oid, qgit_repository *repo,
                            const char *tag_name, const qgit_oid *target,
                            int force);

/**
 * Delete a tag by its short name.
 *
 * Removes refs/tags/<tag_name>. For annotated tags the tag object itself
 * remains in the ODB.
 *
 * @param repo     repository where the tag lives, must not be NULL
 * @param tag_name short tag name to delete, must not be NULL
 * @return 0 on success, -1 if not found or on error and sets errno
 */
QGIT_EXTERN(int) qgit_tag_delete(qgit_repository *repo, const char *tag_name);

/**
 * Build a list of all tag short names in the repository.
 *
 * The output is a newly allocated vector of char* entries. The caller
 * must free the strings and the vector with vec_free when done.
 *
 * @param out  output pointer to receive a vector of char*, must not be NULL
 * @param repo repository to list tags from, must not be NULL
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int) qgit_tag_list(struct vector **out, qgit_repository *repo);

QGIT_END_DECLS

#endif
