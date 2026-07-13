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

#ifndef LIBQGIT_REPO_INDEX_H
#define LIBQGIT_REPO_INDEX_H

#include <libqgit/common.h>
#include <libqgit/oid.h>
#include <libqgit/types.h>
#include <stddef.h>
#include <time.h>

QGIT_BEGIN_DECLS

/* Masks and shifts for the flags field of qgit_index_entry. */
#define QGIT_IDXENTRY_NAMEMASK 0x0fff  /* low 12 bits store the path length */
#define QGIT_IDXENTRY_STAGEMASK 0x3000 /* bits 12-13 store the merge stage */
#define QGIT_IDXENTRY_STAGESHIFT                                               \
    12 /* right-shift flags by this to get stage                               \
        */

#define QGIT_IDXENTRY_STAGE_NORMAL 0 /* base stage */
#define QGIT_IDXENTRY_STAGE_BASE 1   /* merge base stage */
#define QGIT_IDXENTRY_STAGE_OURS 2
#define QGIT_IDXENTRY_STAGE_THEIRS 3

/* Stat timestamp stored per-entry for change detection. */
typedef struct {
    time_t seconds;           /* whole seconds */
    unsigned int nanoseconds; /* sub-second component */
} qgit_index_time;

/* A single entry in the index representing one tracked path. */
typedef struct {
    qgit_index_time ctime;  /* last status-change time of the file */
    qgit_index_time mtime;  /* last modification time of the file */
    unsigned int dev;       /* device number of the file */
    unsigned int ino;       /* inode number of the file */
    unsigned int mode;      /* UNIX file mode */
    unsigned int uid;       /* owner user ID */
    unsigned int gid;       /* owner group ID */
    unsigned int file_size; /* on-disk file size in bytes */
    qgit_oid oid;           /* blob OID for this entry */
    unsigned short flags;   /* stage and name-length flags (QGIT_IDXENTRY_*) */
    char *path;             /* null-terminated repository-relative path */
} qgit_index_entry;

/**
 * Open an index file from disk.
 *
 * Use qgit_repository_index to obtain the index associated with a
 * repository. This function is for accessing a standalone index file.
 *
 * @param out        output pointer to receive the index handle, must not be
 * NULL
 * @param index_path path to the index file on disk
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int) qgit_index_open(qgit_index **out, const char *index_path);

/**
 * Remove all entries from an index object in memory.
 *
 * Changes are not written to disk until qgit_index_write is called.
 *
 * @param index index to clear, must not be NULL
 */
QGIT_EXTERN(void) qgit_index_clear(qgit_index *index);

/**
 * Free an index handle and all associated resources.
 *
 * @param index index to free, no-op if NULL
 */
QGIT_EXTERN(void) qgit_index_free(qgit_index *index);

/**
 * Reload the index from disk, discarding any in-memory changes.
 *
 * @param index index to reload, must not be NULL
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int) qgit_index_read(qgit_index *index);

/**
 * Write the in-memory index state back to disk atomically.
 *
 * @param index index to write, must not be NULL
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int) qgit_index_write(qgit_index *index);

/**
 * Return the number of entries currently in the index.
 *
 * @param index index to query, must not be NULL
 * @return entry count
 */
QGIT_EXTERN(unsigned int) qgit_index_entrycount(qgit_index *index);

/**
 * Return a pointer to the entry at position n.
 *
 * The returned pointer is owned by the index and is valid until the
 * index is modified or freed. Changes made through this pointer will
 * be written on the next qgit_index_write call.
 *
 * @param index index to query, must not be NULL
 * @param n     zero-based entry position
 * @return pointer to the entry, or NULL if n is out of range
 */
QGIT_EXTERN(qgit_index_entry *)
qgit_index_get(qgit_index *index, unsigned int n);

/**
 * Find the position of the first entry matching path.
 *
 * @param index index to search, must not be NULL
 * @param path  repository-relative path to find
 * @return position >= 0 if found, -1 if not found
 */
QGIT_EXTERN(int) qgit_index_find(qgit_index *index, const char *path);

/**
 * Find the position of the entry matching path and stage.
 *
 * @param index index to search, must not be NULL
 * @param path  repository-relative path to find
 * @param stage merge stage number (0 for a normal entry, 1-3 for a conflict)
 * @return position >= 0 if found, -1 if not found
 */
QGIT_EXTERN(int)
qgit_index_find2(qgit_index *index, const char *path, int stage);

/**
 * Add or update an index entry from a file in the working directory.
 *
 * Reads the file at path (relative to the repository working directory),
 * writes it to the ODB as a blob, and records or updates the index entry.
 * If an entry for the same path and stage already exists it is replaced.
 *
 * @param index index to modify, must not be NULL
 * @param path  repository-relative path of the file to stage
 * @param stage merge stage number (0 for a normal staged file)
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int) qgit_index_add(qgit_index *index, const char *path, int stage);

/**
 * Add or update an index entry from an in-memory entry struct.
 *
 * A full copy of source_entry (including the path string) is stored.
 * If an entry for the same path and stage already exists it is replaced.
 *
 * @param index        index to modify, must not be NULL
 * @param source_entry entry to copy into the index, must not be NULL
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int)
qgit_index_add2(qgit_index *index, const qgit_index_entry *source_entry);

/**
 * Remove the entry at the given position.
 *
 * @param index    index to modify, must not be NULL
 * @param position zero-based position of the entry to remove
 * @return 0 on success, -1 if position is out of range or on error and sets
 * errno
 */
QGIT_EXTERN(int) qgit_index_remove(qgit_index *index, unsigned int position);

/**
 * Extract the merge stage number from an entry's flags field.
 *
 * @param entry entry to inspect, must not be NULL
 * @return stage number (0 for a normal entry, 1-3 for a conflicted entry)
 */
QGIT_EXTERN(int) qgit_index_entry_stage(const qgit_index_entry *entry);

/**
 * Replace all index entries with the contents of a tree.
 *
 * Used during checkout to populate the index from a commit's tree.
 * All existing entries are discarded.
 *
 * @param index index to populate, must not be NULL
 * @param tree  tree to read entries from, must not be NULL
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int) qgit_index_read_tree(qgit_index *index, qgit_tree *tree);

QGIT_END_DECLS

#endif
