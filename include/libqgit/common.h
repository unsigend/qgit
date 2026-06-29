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

#include <sys/stat.h>

#ifdef __cplusplus
#define BEGIN_DECLS extern "C" {
#define END_DECLS }
#else
#define BEGIN_DECLS
#define END_DECLS
#endif

#ifdef __GNUC__
#define QGIT_EXTERN(type) extern __attribute__((visibility("default"))) type
#elif defined(_MSC_VER)
#define QGIT_EXTERN(type) extern __declspec(dllexport) type
#else
#define QGIT_EXTERN(type) extern type
#endif

#ifdef __GNUC__
#define QGIT_INTERNAL(type) __attribute__((visibility("hidden"))) type
#else
#define QGIT_INTERNAL(type) type
#endif

#ifdef __GNUC__
#define QGIT_INLINE(type) __attribute__((always_inline)) static inline type
#elif defined(_MSC_VER)
#define QGIT_INLINE(type) __inline static type
#else
#define QGIT_INLINE(type) static inline type
#endif

#define QGIT_UNUSED(x) (void)(x)
#define QGIT_FORMAT(fmt, args) __attribute__((format(printf, fmt, args)))

#define QGIT_DIR_MODE (S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)
#define QGIT_FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

#endif