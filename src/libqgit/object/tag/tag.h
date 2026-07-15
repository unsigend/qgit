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

#ifndef TAG_H
#define TAG_H

#include "../object/object.h"

#include <libqgit/error.h>
#include <string.h>

/* Raw payload format for annotated tag:
     object <sha1-40>\n
     type <commit|tree|blob|tag>\n
     tag <name>\n
     tagger <name> <email> <timestamp> <timezone>\n
     \n
     <message>
*/

struct qgit_tag {
    qgit_object object;
    qgit_oid target_oid;
    qgit_obj_type target_type;
    char *tag_name;
    qgit_signature *tagger;
    char *message;
};

/**
 * Parse a tag object from an ODB object.
 *
 * @param out output pointer to receive the tag object, must not be NULL
 * @param odb_obj ODB object to parse from, must not be NULL
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_INTERNAL(int) tag_parse(qgit_tag *out, qgit_odb_object *odb_obj);

/**
 * Free a tag object.
 *
 * @param tag tag object to free
 */
QGIT_INTERNAL(void) tag_free(qgit_tag *tag);

/**
 * Validate a tag name.
 *
 * @param name The tag name to validate.
 * @return 0 if the name is valid, -1 if the name is invalid.
 */
QGIT_INLINE(int) qgit_tag_validate_name(const char *name)
{
    if (!name[0] || strstr(name, "..") || name[0] == '/') {
        qgit_seterror(QGITERR_BADTAGNAME);
        return -1;
    }
    return 0;
}

#endif