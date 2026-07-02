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

#ifndef LIBQGIT_CONFIG_H
#define LIBQGIT_CONFIG_H

#include <libqgit/common.h>
#include <libqgit/types.h>
#include <stdio.h>

BEGIN_DECLS

/**
 * Create a new config instance containing a single on-disk file.
 *
 * @param out pointer to store the config instance
 * @param path path to the on-disk file to open
 * @return 0 on success, -1 on error and set errno
 */
QGIT_EXTERN(int) qgit_config_open(qgit_config **out, const char *path);

/**
 * Open the global configuration file.
 *
 * @param out pointer to store the config instance
 * @return 0 on success, -1 on error and set errno
 */
QGIT_EXTERN(int) qgit_config_open_global(qgit_config **out);

/**
 * Locate the path to the global configuration file.
 *
 * @param path buffer to store the path
 * @param size size of the `path` buffer
 * @return 0 on success, -1 on error and set errno
 */
QGIT_EXTERN(int) qgit_config_global_path(char *path, size_t size);

/**
 * Create a new empty in-memory config associated with a path.
 *
 * The config is not written to disk until `qgit_config_write()` is called.
 *
 * @param out pointer to the new configuration
 * @param path path associated with the config
 * @return 0 on success, -1 on error and set errno
 */
QGIT_EXTERN(int) qgit_config_create(qgit_config **out, const char *path);

/**
 * Write all pending changes back to disk.
 *
 * @param config the configuration to write
 * @return 0 on success, -1 on error and set errno
 */
QGIT_EXTERN(int) qgit_config_write(qgit_config *config);

/**
 * Free the configuration and its associated memory.
 *
 * @param config the configuration to free
 */
QGIT_EXTERN(void) qgit_config_free(qgit_config *config);

/**
 * Get the value of a string config variable.
 *
 * The string is owned by the config and valid until the config is
 * modified or freed.
 *
 * @param out pointer to the variable's value
 * @param config where to look for the variable
 * @param name the variable's name in "section.key" form
 * @return 0 on success, -1 on error and set errno
 */
QGIT_EXTERN(int)
qgit_config_get_string(const char **out, qgit_config *config, const char *name);

/**
 * Get the value of a boolean config variable.
 *
 * Recognizes "true"/"false", "yes"/"no", and "1"/"0".
 *
 * @param out pointer to the variable where the value should be stored
 * @param config where to look for the variable
 * @param name the variable's name in "section.key" form
 * @return 0 on success, -1 on error and set errno
 */
QGIT_EXTERN(int)
qgit_config_get_bool(int *out, qgit_config *config, const char *name);

/**
 * Get the value of an integer config variable.
 *
 * @param out pointer to the variable where the value should be stored
 * @param config where to look for the variable
 * @param name the variable's name in "section.key" form
 * @return 0 on success, -1 on error and set errno
 */
QGIT_EXTERN(int)
qgit_config_get_int(int *out, qgit_config *config, const char *name);

/**
 * Set the value of a string config variable.
 *
 * An internal copy of the value is made.
 *
 * @param config where to look for the variable
 * @param name the variable's name in "section.key" form
 * @param value string value for the variable
 * @return 0 on success, -1 on error and set errno
 */
QGIT_EXTERN(int)
qgit_config_set_string(qgit_config *config, const char *name,
                       const char *value);

/**
 * Set the value of a boolean config variable.
 *
 * The value is stored as "true" or "false".
 *
 * @param config where to look for the variable
 * @param name the variable's name in "section.key" form
 * @param value the value to store
 * @return 0 on success, -1 on error and set errno
 */
QGIT_EXTERN(int)
qgit_config_set_bool(qgit_config *config, const char *name, int value);

/**
 * Set the value of an integer config variable.
 *
 * @param config where to look for the variable
 * @param name the variable's name in "section.key" form
 * @param value integer value for the variable
 * @return 0 on success, -1 on error and set errno
 */
QGIT_EXTERN(int)
qgit_config_set_int(qgit_config *config, const char *name, int value);

/**
 * Delete a config entry.
 *
 * @param config where to look for the variable
 * @param name the variable's name in "section.key" form
 * @return 0 on success, -1 on error and set errno
 */
QGIT_EXTERN(int) qgit_config_delete(qgit_config *config, const char *name);

/**
 * Print the config to an output stream.
 *
 * Each entry is printed in the form [section].[key]=[value].
 *
 * @param config the configuration to print
 * @param stream output stream
 * @return 0 on success, -1 on error and set errno
 */
QGIT_EXTERN(int) qgit_config_fprint(const qgit_config *config, FILE *stream);

END_DECLS

#endif
