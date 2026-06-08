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

#include "sha1.h"

int sha1(const void *data, size_t len, unsigned char out[SHA1_DIGEST_LENGTH])
{
  EVP_MD_CTX *ctx = EVP_MD_CTX_new();
  if (!ctx)
    return -1;

  int ok = EVP_DigestInit_ex(ctx, EVP_sha1(), NULL) &&
           EVP_DigestUpdate(ctx, data, len) &&
           EVP_DigestFinal_ex(ctx, out, NULL);

  EVP_MD_CTX_free(ctx);
  return ok ? 0 : -1;
}

int sha1_to_hex(const unsigned char *in, unsigned char out[SHA1_HEX_LENGTH])
{
  for (size_t i = 0; i < SHA1_DIGEST_LENGTH; i++) {
    if (snprintf((char *)(out + i * 2), 3, "%02x", in[i]) < 0)
      return -1;
  }
  out[SHA1_HEX_LENGTH - 1] = '\0';
  return 0;
}

static int hexval(unsigned char c)
{
  if (c >= '0' && c <= '9')
    return c - '0';
  if (c >= 'a' && c <= 'f')
    return c - 'a' + 10;
  if (c >= 'A' && c <= 'F')
    return c - 'A' + 10;
  return -1;
}

int hex_to_sha1(const unsigned char *hex, unsigned char out[SHA1_DIGEST_LENGTH])
{
  for (size_t i = 0; i < SHA1_DIGEST_LENGTH; i++) {
    int hi = hexval(hex[i * 2]);
    int lo = hexval(hex[i * 2 + 1]);
    if (hi == -1 || lo == -1)
      return -1;
    out[i] = (unsigned char)(hi << 4 | lo);
  }
  return 0;
}