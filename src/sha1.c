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

#include <openssl/evp.h>
#include <sha1.h>

int sha1(const void *data, size_t len, unsigned char sha1[SHA1_DIGLEN])
{
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    if (!ctx)
        return -1;

    int ok = EVP_DigestInit_ex(ctx, EVP_sha1(), NULL) &&
             EVP_DigestUpdate(ctx, data, len) &&
             EVP_DigestFinal_ex(ctx, sha1, NULL);

    EVP_MD_CTX_free(ctx);
    return ok ? 0 : -1;
}
