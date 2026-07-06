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
#include <libqgit/object/signature.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int qgit_signature_now(qgit_signature **out, const char *name,
                       const char *email)
{
    assert(out && name && email);
    *out = NULL;

    struct tm utc_tm;
    int offset;
    time_t now = time(NULL);

    if (gmtime_r(&now, &utc_tm) == NULL)
        return -1;
    utc_tm.tm_isdst = -1;
    offset = (int)(difftime(now, mktime(&utc_tm)) / 60);

    return qgit_signature_new(out, name, email, now, offset);
}
