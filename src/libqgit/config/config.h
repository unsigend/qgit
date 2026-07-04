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

#ifndef CONFIG_H
#define CONFIG_H

#include <errno.h>
#include <iniparse.h>
#include <libqgit/common.h>
#include <libqgit/error.h>
#include <string.h>

struct qgit_config {
    struct iniFILE *inifp;
};

/**
 * Parse a section and key from a name.
 *
 * @param name The name to parse in "section.key" form.
 * @param sec Output pointer to receive the section, must not be NULL
 * @param key Output pointer to receive the key, must not be NULL
 * @return 0 on success, -1 on error and sets errno
 */
QGIT_INLINE(int)
parse_seckey(char *name, char **sec, char **key)
{
    *sec = NULL;
    *key = NULL;
    char *dot = strchr(name, '.');
    if (!dot || dot == name || *(dot + 1) == '\0') {
        qgit_seterror(QGITERR_INVKEY);
        return -1;
    }
    *dot++ = '\0';
    *sec = name;
    *key = dot;
    return 0;
}

#endif