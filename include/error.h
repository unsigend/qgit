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

#ifndef ERROR_H
#define ERROR_H

#include <errno.h>

#define QE_NOTINREPO 1       /* not inside a qgit repository */
#define QE_BADOBJFILE 2      /* bad object file */
#define QE_INTERNAL 3        /* internal error */
#define QE_AMBIGUOUS 4       /* ambiguous argument */
#define QE_INVALIDOBJ 5      /* invalid object */
#define QE_EXISTSTAG 6       /* tag already exists */
#define QE_BADSIGN 7         /* bad signature field */
#define QE_IDENTITY 8        /* identity is required */
#define QE_PEEL 9            /* could not peel object */
#define QE_BADREV 10         /* bad revision */
#define QE_NOIDENTITY 11     /* author identity unknown */
#define QE_EXISTBRANCH 12    /* branch already exists */
#define QE_BADIDXMAGIC 13    /* bad index magic number */
#define QE_BADIDXVERSION 14  /* bad index version */
#define QE_BADIDX 15         /* bad index file */
#define QE_IDXSUPPORTS 16    /* index format not supported */
#define QE_BADIDXCHECKSUM 17 /* bad index checksum */

/* wrapper for get_qerror(), follow ANSI/ISO C errno design pattern.*/
extern int *qerrno_location(void);
#define qerrno (*qerrno_location())
#define setqerrno(code)                                                        \
  errno = 0;                                                                   \
  qerrno = code;

extern const char *qerror_str(int error);

#endif