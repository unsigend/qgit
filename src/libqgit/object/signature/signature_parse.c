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

#include <assert.h>
#include <errno.h>
#include <libqgit/error.h>
#include <libqgit/object/signature.h>
#include <stdlib.h>
#include <string.h>

static int offset_from_timezone(const char *zone)
{
    int offset = zone[0] == '+' ? 1 : -1;
    int hours = (zone[1] - '0') * 10 + (zone[2] - '0');
    int minutes = (zone[3] - '0') * 10 + (zone[4] - '0');
    return offset * (hours * 60 + minutes);
}

char *qgit_signature_parse(qgit_signature **out, char *start, char *end)
{
    assert(out && start && end);

    *out = NULL;

    char *p = start;
    const char *name, *email, *zone;
    char *endstr;
    unsigned long timestamp;

    while (p < end && *p != '<')
        p++;

    if (p >= end) {
        qgit_seterror(QGITERR_BADSIGNATURE);
        return NULL;
    }

    *(p - 1) = '\0';
    p++;

    name = start;
    email = p;

    while (p < end && *p != '>')
        p++;
    if (p >= end) {
        qgit_seterror(QGITERR_BADSIGNATURE);
        return NULL;
    }

    *p++ = '\0';

    errno = 0;
    timestamp = strtoul(++p, &endstr, 10);
    if (endstr == p || *endstr != ' ' || errno) {
        if (!errno)
            qgit_seterror(QGITERR_BADSIGNATURE);
        return NULL;
    }

    p = endstr;
    *p++ = '\0';

    zone = p;

    while (p < end && *p != '\n')
        p++;
    if (p >= end) {
        qgit_seterror(QGITERR_BADSIGNATURE);
        return NULL;
    }

    *p++ = '\0';
    if (strlen(zone) < 5) {
        qgit_seterror(QGITERR_BADSIGNATURE);
        return NULL;
    }

    int ret = qgit_signature_new(out, name, email, timestamp,
                                 offset_from_timezone(zone));
    if (ret)
        return NULL;
    return p;
}