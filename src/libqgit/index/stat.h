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

#ifndef QGIT_STAT_H
#define QGIT_STAT_H

#include <sys/stat.h>

/* Crossplatform function to get the nanoseconds of the ctime */
__attribute__((always_inline)) inline long
stat_ctime_nsec(const struct stat *st)
{
#if defined(__APPLE__) || defined(__NetBSD__) || defined(__OpenBSD__)
    return st->st_ctimespec.tv_nsec;
#elif defined(__linux__)
    return st->st_ctim.tv_nsec;
#else
    return 0;
#endif
}

/* Crossplatform function to get the nanoseconds of the mtime */
__attribute__((always_inline)) inline long
stat_mtime_nsec(const struct stat *st)
{
#if defined(__APPLE__) || defined(__NetBSD__) || defined(__OpenBSD__)
    return st->st_mtimespec.tv_nsec;
#elif defined(__linux__)
    return st->st_mtim.tv_nsec;
#else
    return 0;
#endif
}

#endif