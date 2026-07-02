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

#ifndef LIBQGIT_REFS_H
#define LIBQGIT_REFS_H

#include <libqgit/common.h>
#include <libqgit/db/oid.h>
#include <libqgit/types.h>

BEGIN_DECLS

/**
 * Lookup a reference by its name in a repository.
 *
 * The generated reference must be freed by the user.
 *
 * @param out pointer to the looked-up reference
 * @param repo the repository to look up the reference
 * @param name the long name for the reference (e.g. HEAD, refs/heads/main)
 * @return 0 on success, -1 on error and set errno
 */
QGIT_EXTERN(int)
qgit_reference_lookup(qgit_reference **out, qgit_repository *repo,
                      const char *name);

/**
 * Lookup a reference by name and resolve immediately to OID.
 *
 * Convenience wrapper for lookup, resolve, and oid retrieval.
 *
 * @param out pointer to oid to be filled in
 * @param repo the repository in which to look up the reference
 * @param name the long name for the reference
 * @return 0 on success, -1 on error and set errno
 */
QGIT_EXTERN(int)
qgit_reference_name_to_oid(qgit_oid *out, qgit_repository *repo,
                           const char *name);

/**
 * Create a new object id reference.
 *
 * The reference will be created in the repository and written
 * to the disk.
 *
 * The generated reference must be freed by the user.
 *
 * If `force` is true and there already exists a reference
 * with the same name, it will be overwritten.
 *
 * @param out pointer to the newly created reference
 * @param repo repository where that reference will live
 * @param name the name of the reference
 * @param id the object id pointed to by the reference
 * @param force overwrite existing references
 * @return 0 on success, -1 on error and set errno
 */
QGIT_EXTERN(int)
qgit_reference_create_oid(qgit_reference **out, qgit_repository *repo,
                          const char *name, const qgit_oid *id, int force);

/**
 * Create a new symbolic reference.
 *
 * The reference will be created in the repository and written
 * to the disk.
 *
 * The generated reference must be freed by the user.
 *
 * If `force` is true and there already exists a reference
 * with the same name, it will be overwritten.
 *
 * @param out pointer to the newly created reference
 * @param repo repository where that reference will live
 * @param name the name of the reference
 * @param target the target of the reference
 * @param force overwrite existing references
 * @return 0 on success, -1 on error and set errno
 */
QGIT_EXTERN(int)
qgit_reference_create_symbolic(qgit_reference **out, qgit_repository *repo,
                               const char *name, const char *target, int force);

/**
 * Get the OID pointed to by a reference.
 *
 * Only available if the reference is direct (i.e. not symbolic).
 *
 * @param ref the reference
 * @return a pointer to the oid if available, NULL otherwise
 */
QGIT_EXTERN(const qgit_oid *) qgit_reference_oid(const qgit_reference *ref);

/**
 * Get full name to the reference pointed by this reference.
 *
 * Only available if the reference is symbolic.
 *
 * @param ref the reference
 * @return a pointer to the name if available, NULL otherwise
 */
QGIT_EXTERN(const char *) qgit_reference_target(const qgit_reference *ref);

/**
 * Get the type of a reference.
 *
 * Either direct (QGIT_REF_OID) or symbolic (QGIT_REF_SYMBOLIC).
 *
 * @param ref the reference
 * @return the type
 */
QGIT_EXTERN(qgit_ref_type) qgit_reference_type(const qgit_reference *ref);

/**
 * Get the full name of a reference.
 *
 * @param ref the reference
 * @return the full name for the ref
 */
QGIT_EXTERN(const char *) qgit_reference_name(const qgit_reference *ref);

/**
 * Get the repository where a reference resides.
 *
 * @param ref the reference
 * @return a pointer to the repo
 */
QGIT_EXTERN(qgit_repository *) qgit_reference_owner(const qgit_reference *ref);

/**
 * Resolve a symbolic reference.
 *
 * This method iteratively peels a symbolic reference
 * until it resolves to a direct reference to an OID.
 *
 * The peeled reference is returned in the `out`
 * argument, and must be freed manually once it is no longer
 * needed.
 *
 * If a direct reference is passed as an argument,
 * a copy of that reference is returned. This copy must
 * be manually freed too.
 *
 * @param out pointer to the peeled reference
 * @param ref the reference
 * @return 0 on success, -1 on error and set errno
 */
QGIT_EXTERN(int)
qgit_reference_resolve(qgit_reference **out, const qgit_reference *ref);

/**
 * Set the OID target of a reference.
 *
 * The reference must be a direct reference, otherwise
 * this method will fail.
 *
 * The reference will be automatically updated in
 * memory and on disk.
 *
 * @param ref the reference
 * @param id the new target OID for the reference
 * @return 0 on success, -1 on error and set errno
 */
QGIT_EXTERN(int)
qgit_reference_set_oid(qgit_reference *ref, const qgit_oid *id);

/**
 * Set the symbolic target of a reference.
 *
 * The reference must be a symbolic reference, otherwise
 * this method will fail.
 *
 * The reference will be automatically updated in
 * memory and on disk.
 *
 * @param ref the reference
 * @param target the new target for the reference
 * @return 0 on success, -1 on error and set errno
 */
QGIT_EXTERN(int)
qgit_reference_set_target(qgit_reference *ref, const char *target);

/**
 * Delete an existing reference.
 *
 * This method works for both direct and symbolic references.
 *
 * The reference will be immediately removed on disk and from
 * memory. The given reference pointer will no longer be valid.
 *
 * @param ref the reference to remove
 * @return 0 on success, -1 on error and set errno
 */
QGIT_EXTERN(int) qgit_reference_delete(qgit_reference *ref);

/**
 * Free the given reference.
 *
 * @param ref qgit_reference
 */
QGIT_EXTERN(void) qgit_reference_free(qgit_reference *ref);

/**
 * Perform an operation on each reference in the repository.
 *
 * The `callback` function will be called for each of the references
 * in the repository, and will receive the name of the reference and
 * the `payload` value passed to this method.
 *
 * @param repo repository where to find the refs
 * @param callback function which will be called for every listed ref
 * @param payload additional data to pass to the callback
 * @return 0 on success, -1 on error and set errno
 */
QGIT_EXTERN(int)
qgit_reference_foreach(qgit_repository *repo,
                       int (*callback)(const char *name, void *payload),
                       void *payload);

END_DECLS

#endif
