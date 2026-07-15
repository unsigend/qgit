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

#ifndef LIBQGIT_REPO_REFS_H
#define LIBQGIT_REPO_REFS_H

#include <collection/vector.h>
#include <libqgit/oid.h>

QGIT_BEGIN_DECLS

/* Flags for qgit_reference_list, combinable with bitwise OR. */
#define QGIT_REF_LIST_BRANCHES (1u << 0) /* limit to refs/heads/ */
#define QGIT_REF_LIST_TAGS (1u << 1)     /* limit to refs/tags/ */
#define QGIT_REF_LIST_REMOTES (1u << 2)  /* limit to refs/remotes/ */
#define QGIT_REF_LIST_ALL                                                      \
    (QGIT_REF_LIST_BRANCHES | QGIT_REF_LIST_TAGS |                             \
     QGIT_REF_LIST_REMOTES) /* list all refs under refs/ */

/**
 * Look up a reference by its full name.
 *
 * @param out  output pointer to receive the reference handle, must not be NULL
 * @param repo repository to search, must not be NULL
 * @param name full reference name (e.g. "HEAD", "refs/heads/main",
 * "refs/tags/v1.0")
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int)
qgit_reference_lookup(qgit_reference **out, qgit_repository *repo,
                      const char *name);

/**
 * Resolve a reference name directly to an OID.
 *
 * Symbolic references are followed until a direct reference is reached.
 *
 * @param out  output pointer to receive the resolved OID, must not be NULL
 * @param repo repository to search, must not be NULL
 * @param name full reference name to resolve
 * @return 0 on success, -1 if the name cannot be resolved and sets errno
 */
QGIT_EXTERN(int)
qgit_reference_name_to_oid(qgit_oid *out, qgit_repository *repo,
                           const char *name);

/**
 * Create a new symbolic reference and write it to disk.
 *
 * @param out    output pointer to receive the new reference handle, must not be
 *               NULL
 * @param repo   repository where the reference will be created, must not be
 *               NULL
 * @param name   full name of the new reference (e.g. "HEAD")
 * @param target full name of the reference this one points to
 * @param force  if non-zero, overwrite any existing reference with this name
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int)
qgit_reference_create_symbolic(qgit_reference **out, qgit_repository *repo,
                               const char *name, const char *target, int force);

/**
 * Create a new direct reference pointing to an OID and write it to disk.
 *
 * @param out   output pointer to receive the new reference handle, must not be
 *              NULL
 * @param repo  repository where the reference will be created, must not be NULL
 * @param name  full name of the new reference (e.g. "refs/heads/main")
 * @param oid   OID the reference will point to, must not be NULL
 * @param force if non-zero, overwrite any existing reference with this name
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int)
qgit_reference_create_oid(qgit_reference **out, qgit_repository *repo,
                          const char *name, const qgit_oid *oid, int force);

/**
 * Get the OID a direct reference points to.
 *
 * Returns NULL if the reference is symbolic.
 *
 * @param ref reference to query, must not be NULL
 * @return pointer to the OID, or NULL if the reference is symbolic
 */
QGIT_EXTERN(const qgit_oid *) qgit_reference_oid(const qgit_reference *ref);

/**
 * Get the target name a symbolic reference points to.
 *
 * Returns NULL if the reference is direct.
 *
 * @param ref reference to query, must not be NULL
 * @return pointer to the target name, or NULL if the reference is direct
 */
QGIT_EXTERN(const char *) qgit_reference_target(const qgit_reference *ref);

/**
 * Get the type of a reference.
 *
 * @param ref reference to query, must not be NULL
 * @return QGIT_REF_DIRECT or QGIT_REF_SYMBOLIC
 */
QGIT_EXTERN(qgit_ref_type) qgit_reference_type(const qgit_reference *ref);

/**
 * Get the full name of a reference.
 *
 * The returned pointer is owned by the reference and is valid until
 * qgit_reference_free is called.
 *
 * @param ref reference to query, must not be NULL
 * @return the full reference name (e.g. "refs/heads/main")
 */
QGIT_EXTERN(const char *) qgit_reference_name(const qgit_reference *ref);

/**
 * Resolve a symbolic reference to the direct reference it ultimately targets.
 *
 * If ref is already direct, a copy is returned. The returned reference
 * must be freed with qgit_reference_free.
 *
 * @param out output pointer to receive the resolved direct reference, must not
 * be NULL
 * @param ref reference to resolve, must not be NULL
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int)
qgit_reference_resolve(qgit_reference **out, const qgit_reference *ref);

/**
 * Get the repository that owns a reference.
 *
 * @param ref reference to query, must not be NULL
 * @return pointer to the owning repository
 */
QGIT_EXTERN(qgit_repository *) qgit_reference_owner(const qgit_reference *ref);

/**
 * Update the target of a symbolic reference in memory and on disk.
 *
 * The reference must be symbolic, otherwise the call fails.
 *
 * @param ref    reference to update, must not be NULL
 * @param target new target reference name
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int)
qgit_reference_set_target(qgit_reference *ref, const char *target);

/**
 * Update the OID target of a direct reference in memory and on disk.
 *
 * The reference must be direct, otherwise the call fails.
 *
 * @param ref reference to update, must not be NULL
 * @param oid new OID the reference will point to, must not be NULL
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int)
qgit_reference_set_oid(qgit_reference *ref, const qgit_oid *oid);

/**
 * Delete a reference from disk and free it.
 *
 * The reference pointer is invalid after this call.
 *
 * @param ref reference to delete, must not be NULL
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int) qgit_reference_delete(qgit_reference *ref);

/**
 * Free a reference handle.
 *
 * @param ref reference to free, no-op if NULL
 */
QGIT_EXTERN(void) qgit_reference_free(qgit_reference *ref);

/**
 * Compare two references by name.
 *
 * @param a first reference, must not be NULL
 * @param b second reference, must not be NULL
 * @return 0 if equal, non-zero otherwise
 */
QGIT_EXTERN(int) qgit_reference_cmp(qgit_reference *a, qgit_reference *b);

/**
 * Build a list of reference names in the repository.
 *
 * The output is a newly allocated vector of char* entries, one per
 * reference name. The caller is responsible for freeing both the strings
 * and the vector. Use vec_free to release it.
 *
 * flags controls which references are included, use QGIT_REF_LIST_ALL
 * to include every reference under refs/, or combine QGIT_REF_LIST_BRANCHES,
 * QGIT_REF_LIST_REMOTES and QGIT_REF_LIST_TAGS for a filtered listing.
 *
 * @param out   output pointer to receive a vector of char*, must not be NULL
 * @param repo  repository to list references from, must not be NULL
 * @param flags bitwise OR of QGIT_REF_LIST_* flags
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int)
qgit_reference_list(struct vector **out, qgit_repository *repo,
                    unsigned int flags);

QGIT_END_DECLS

#endif
