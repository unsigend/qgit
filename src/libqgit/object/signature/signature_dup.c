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

qgit_signature *qgit_signature_dup(const qgit_signature *sig)
{
    assert(sig);

    qgit_signature *dupsig;
    dupsig = calloc(1, sizeof(qgit_signature));
    if (!dupsig)
        return NULL;

    dupsig->email = strdup(sig->email);
    if (!dupsig->email) {
        qgit_signature_free(dupsig);
        return NULL;
    }

    dupsig->name = strdup(sig->name);
    if (!dupsig->name) {
        qgit_signature_free(dupsig);
        return NULL;
    }

    dupsig->when.time = sig->when.time;
    dupsig->when.offset = sig->when.offset;

    return dupsig;
}
