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

int qgit_signature_new(qgit_signature **out, const char *name,
                       const char *email, time_t time, int offset)
{
    assert(out && name && email);

    *out = NULL;

    qgit_signature *signature;

    signature = calloc(1, sizeof(qgit_signature));
    if (!signature)
        return -1;

    signature->email = strdup(email);
    if (!signature->email) {
        qgit_signature_free(signature);
        return -1;
    }

    signature->name = strdup(name);
    if (!signature->name) {
        qgit_signature_free(signature);
        return -1;
    }

    signature->when.time = time;
    signature->when.offset = offset;

    *out = signature;

    return 0;
}
