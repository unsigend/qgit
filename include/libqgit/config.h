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

/* Open and parse an existing config file at path. */
QGIT_EXTERN(int) qgit_config_open(qgit_config **out, const char *path);

/* Open and parse the global config file. */
QGIT_EXTERN(int) qgit_config_open_global(qgit_config **out);

/* Write the path to the global config file into path (size bytes). */
QGIT_EXTERN(int) qgit_config_global_path(char *path, size_t size);

/* Create a new empty in-memory config associated with path not written until
   qgit_config_write is called. */
QGIT_EXTERN(int) qgit_config_create(qgit_config **out, const char *path);

/* Write all pending changes back to disk. */
QGIT_EXTERN(int) qgit_config_write(qgit_config *config);

/* Free a previously allocated config. */
QGIT_EXTERN(void) qgit_config_free(qgit_config *config);

/* Get a string value for key "section.key". The returned pointer is owned by
   the config and valid until the config is modified or freed. */
QGIT_EXTERN(int)
qgit_config_get_string(const char **out, qgit_config *config, const char *name);

/* Get a boolean value. Recognizes "true"/"false", "yes"/"no", "1"/"0". */
QGIT_EXTERN(int)
qgit_config_get_bool(int *out, qgit_config *config, const char *name);

/* Get an integer value. */
QGIT_EXTERN(int)
qgit_config_get_int(int *out, qgit_config *config, const char *name);

/* Set a string value for key "section.key". An internal copy is made. */
QGIT_EXTERN(int)
qgit_config_set_string(qgit_config *config, const char *name,
                       const char *value);

/* Set a boolean value. Stored as "true" or "false". */
QGIT_EXTERN(int)
qgit_config_set_bool(qgit_config *config, const char *name, int value);

/* Set an integer value. */
QGIT_EXTERN(int)
qgit_config_set_int(qgit_config *config, const char *name, int value);

/* Delete a config entry. name is "section.key". */
QGIT_EXTERN(int) qgit_config_delete(qgit_config *config, const char *name);

/* Print the config with the format of [section].[key]=[value] to the output
   stream. */
QGIT_EXTERN(int) qgit_config_fprint(const qgit_config *config, FILE *stream);

END_DECLS

#endif