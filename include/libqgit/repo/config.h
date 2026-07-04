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

#ifndef LIBQGIT_REPO_CONFIG_H
#define LIBQGIT_REPO_CONFIG_H

#include <libqgit/common.h>
#include <libqgit/types.h>
#include <stddef.h>
#include <stdio.h>

QGIT_BEGIN_DECLS

/**
 * Locate the global config file (~/.qgitconfig).
 *
 * @param out      buffer to receive the absolute path
 * @param out_size size of the buffer in bytes
 * @return 0 on success, -1 if not found or on error and sets errno
 */
QGIT_EXTERN(int) qgit_config_find_global(char *out, size_t out_size);

/**
 * Open the global config file (~/.qgitconfig).
 *
 * @param out output pointer to receive the config handle, must not be NULL
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int) qgit_config_open_global(qgit_config **out);

/**
 * Create the global config file (~/.qgitconfig).
 *
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int) qgit_config_create_global(qgit_config **out);

/**
 * Open a single on-disk config file.
 *
 * @param out  output pointer to receive the config handle, must not be NULL
 * @param path path to the config file to open
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int) qgit_config_open_ondisk(qgit_config **out, const char *path);

/**
 * Free a config handle and all associated resources.
 *
 * Pending in-memory changes that have not been saved with qgit_config_save
 * are discarded.
 *
 * @param cfg config to free, no-op if NULL
 */
QGIT_EXTERN(void) qgit_config_free(qgit_config *cfg);

/**
 * Flush in-memory config changes back to the file it was opened from.
 *
 * @param cfg config to save, must not be NULL
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int) qgit_config_save(qgit_config *cfg);

/**
 * Get the value of a string config variable.
 *
 * The returned pointer is owned by the config object and is valid until
 * the config is freed or the variable is modified.
 *
 * @param out  output pointer to receive the string value, must not be NULL
 * @param cfg  config to query, must not be NULL
 * @param name variable name in "section.key" form (e.g. "user.name")
 * @return 0 on success, -1 if not found or on error and sets errno
 */
QGIT_EXTERN(int)
qgit_config_get_string(const char **out, qgit_config *cfg, const char *name);

/**
 * Get the value of a boolean config variable.
 *
 * Accepts true/false, yes/no, on/off, and 1/0 spellings, case-insensitive.
 * The result follows the C convention: 0 is false, 1 is true.
 *
 * @param out  output pointer to receive 0 or 1, must not be NULL
 * @param cfg  config to query, must not be NULL
 * @param name variable name in "section.key" form (e.g. "core.bare")
 * @return 0 on success, -1 if not found or on error and sets errno
 */
QGIT_EXTERN(int)
qgit_config_get_bool(int *out, qgit_config *cfg, const char *name);

/**
 * Set the value of a string config variable.
 *
 * Creates the variable if it does not exist. A copy of value is stored,
 * the caller may free it afterwards. Changes are not written to disk
 * until qgit_config_save is called.
 *
 * @param cfg   config to modify, must not be NULL
 * @param name  variable name in "section.key" form (e.g. "user.email")
 * @param value string value to store, must not be NULL
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_EXTERN(int)
qgit_config_set_string(qgit_config *cfg, const char *name, const char *value);

/**
 * Delete a config variable.
 *
 * Changes are not written to disk until qgit_config_save is called.
 *
 * @param cfg  config to modify, must not be NULL
 * @param name variable name in "section.key" form to remove
 * @return 0 on success, -1 if not found or on error and sets errno
 */
QGIT_EXTERN(int) qgit_config_delete(qgit_config *cfg, const char *name);

/**
 * Print all config variables to a FILE stream.
 *
 * Each variable is printed on its own line in "section.key=value" form,
 * matching the output format of qgit config --list.
 *
 * @param cfg    config to print, must not be NULL
 * @param stream output stream to write to, must not be NULL
 * @return number of bytes written on success, -1 on error and sets errno
 */
QGIT_EXTERN(int) qgit_config_fprint(qgit_config *cfg, FILE *stream);

/**
 * Print all config variables into a caller-supplied buffer.
 *
 * At most size-1 bytes are written and the result is null-terminated.
 * Each variable is formatted as "section.key=value\n".
 *
 * @param cfg  config to print, must not be NULL
 * @param buf  output buffer, must not be NULL
 * @param size size of the buffer in bytes including the null terminator
 * @return number of bytes that would have been written (excluding the null
 *         terminator), or -1 on error and sets errno
 */
QGIT_EXTERN(int) qgit_config_snprint(qgit_config *cfg, char *buf, size_t size);

QGIT_END_DECLS

#endif
