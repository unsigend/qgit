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

#ifndef LIBQGIT_COMMON_H
#define LIBQGIT_COMMON_H

#ifdef __cplusplus
#define QGIT_BEGIN_DECLS extern "C" {
#define QGIT_END_DECLS }
#else
#define QGIT_BEGIN_DECLS
#define QGIT_END_DECLS
#endif

#ifdef __GNUC__
#define QGIT_EXTERN(type) extern __attribute__((visibility("default"))) type
#elif defined(_MSC_VER)
#define QGIT_EXTERN(type) extern __declspec(dllexport) type
#else
#define QGIT_EXTERN(type) extern type
#endif

#if defined(_MSC_VER)
#define QGIT_INLINE(type) static __inline type
#elif defined(__GNUC__)
#define QGIT_INLINE(type) __attribute__((always_inline)) inline type
#else
#define QGIT_INLINE(type) static inline type
#endif

#ifdef __GNUC__
#define QGIT_FORMAT_PRINTF(format, args)                                       \
    __attribute__((format(printf, format, args)))
#else
#define QGIT_FORMAT_PRINTF(format, args) /* empty */
#endif

#define QGIT_UNUSED(x) (void)(x)

#endif