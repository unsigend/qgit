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

/* Lookup a reference by full name (e.g. "HEAD", "refs/heads/main").
   The returned reference must be freed with qgit_reference_free(). */
QGIT_EXTERN(int)
qgit_reference_lookup(qgit_reference **out, qgit_repository *repo,
                      const char *name);

/* Lookup a reference by name and resolve it directly to an OID.
   Convenience wrapper for lookup + resolve + oid. */
QGIT_EXTERN(int)
qgit_reference_name_to_oid(qgit_oid *out, qgit_repository *repo,
                           const char *name);

/* Create a new direct reference (name to OID) and write it to disk.
   If force is non-zero, an existing reference with the same name is
   overwritten. The returned reference must be freed by the caller. */
QGIT_EXTERN(int)
qgit_reference_create_oid(qgit_reference **out, qgit_repository *repo,
                          const char *name, const qgit_oid *id, int force);

/* Create a new symbolic reference (name to target name) and write it to disk.
   If force is non-zero, an existing reference with the same name is
   overwritten. The returned reference must be freed by the caller. */
QGIT_EXTERN(int)
qgit_reference_create_symbolic(qgit_reference **out, qgit_repository *repo,
                               const char *name, const char *target, int force);

/* Return the OID pointed to by a direct reference. Returns NULL if the
   reference is symbolic. */
QGIT_EXTERN(const qgit_oid *) qgit_reference_oid(const qgit_reference *ref);

/* Return the target name of a symbolic reference. Returns NULL if the
   reference is direct. */
QGIT_EXTERN(const char *) qgit_reference_target(const qgit_reference *ref);

/* Return the type of a reference (QGIT_REF_OID or QGIT_REF_SYMBOLIC). */
QGIT_EXTERN(qgit_ref_type) qgit_reference_type(const qgit_reference *ref);

/* Return the full name of a reference (e.g. "refs/heads/main"). */
QGIT_EXTERN(const char *) qgit_reference_name(const qgit_reference *ref);

/* Return the repository that owns this reference. */
QGIT_EXTERN(qgit_repository *) qgit_reference_owner(const qgit_reference *ref);

/* Iteratively resolve a symbolic reference until a direct reference is
   reached. If ref is already direct, a copy is returned. The result must
   be freed with qgit_reference_free(). */
QGIT_EXTERN(int)
qgit_reference_resolve(qgit_reference **out, const qgit_reference *ref);

/* Update the OID target of a direct reference in memory and on disk. */
QGIT_EXTERN(int)
qgit_reference_set_oid(qgit_reference *ref, const qgit_oid *id);

/* Update the symbolic target of a symbolic reference in memory and on disk. */
QGIT_EXTERN(int)
qgit_reference_set_target(qgit_reference *ref, const char *target);

/* Delete a reference from disk */
QGIT_EXTERN(int) qgit_reference_delete(qgit_reference *ref);

/* Free a reference obtained from a lookup or create call. */
QGIT_EXTERN(void) qgit_reference_free(qgit_reference *ref);

/* Iterate over all references in the repository, calling callback(name,
   payload) for each. Stops early if callback returns non-zero. */
QGIT_EXTERN(int)
qgit_reference_foreach(qgit_repository *repo,
                       int (*callback)(const char *name, void *payload),
                       void *payload);

END_DECLS

#endif
