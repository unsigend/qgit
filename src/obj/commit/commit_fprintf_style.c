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

#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "obj/commit.h"
#include "obj/object.h"
#include "obj/sign.h"
#include "sha1.h"

#define ASCII_YELLOW "\033[33m"
#define ASCII_RESET "\033[0m"

#define INDENT_WIDTH 4

static int print_default(struct object *obj, FILE *fp)
{
  unsigned char hex[SHA1_HEXLEN];
  int istty = isatty(fileno(fp));
  struct slist_iter it;
  unsigned char *sha1 = NULL;
  const char *str = obj->commit.msg;
  struct sign *authorsign = &obj->commit.author;
  char datebuf[64];

  if (sha1_to_hex(obj->sha1, hex) == -1)
    return -1;

  /* commit */
  if (fprintf(fp, "%scommit %s%s\n", istty ? ASCII_YELLOW : "", hex,
              istty ? ASCII_RESET : "") < 0)
    return -1;

  /* merge */
  if (slist_size(&obj->commit.parents) > 1) {
    if (fprintf(fp, "Merge: ") < 0)
      return -1;
    if (slist_iter_init(&it, &obj->commit.parents) == -1)
      return -1;
    while (slist_iter_get(&it)) {
      sha1 = slist_iter_get(&it);
      if (sha1_to_hex(sha1, hex) == -1)
        return -1;
      if (fprintf(fp, "%.7s ", hex) < 0)
        return -1;
      slist_iter_inc(&it);
    }
    if (fprintf(fp, "\n") < 0)
      return -1;
  }

  /* author */
  if (fprintf(fp, "Author: %s <%s>\n", authorsign->name, authorsign->email) < 0)
    return -1;

  /* date */
  if (sign_date_fmt(authorsign, datebuf, sizeof(datebuf)) == -1)
    return -1;
  if (fprintf(fp, "Date:   %s %s\n", datebuf, authorsign->zone) < 0)
    return -1;
  if (fputc('\n', fp) == EOF)
    return -1;

  /* message */
  if (str) {
    if (fprintf(fp, "%*s", INDENT_WIDTH, " ") < 0)
      return -1;
    while (*str) {
      if (*str == '\n') {
        if (fputc('\n', fp) == EOF)
          return -1;
        if (*(str + 1)) /* not the last line */
        {
          if (fprintf(fp, "%*s", INDENT_WIDTH, " ") < 0)
            return -1;
        }
      } else {
        if (fputc(*str, fp) == EOF)
          return -1;
      }
      str++;
    }
  }
  if (str && fputc('\n', fp) == EOF)
    return -1;
  return 0;
}

static int print_oneline(struct object *obj, FILE *fp)
{
  unsigned char hex[SHA1_HEXLEN];
  int istty = isatty(fileno(fp));
  const char *str = obj->commit.msg;

  if (sha1_to_hex(obj->sha1, hex) == -1)
    return -1;
  if (fprintf(fp, "%s%.7s%s ", istty ? ASCII_YELLOW : "", hex,
              istty ? ASCII_RESET : "") < 0)
    return -1;
  if (str) {
    while (*str && *str != '\n') { /* first line only */
      if (fputc(*str, fp) == EOF)
        return -1;
      str++;
    }
  }
  if (fputc('\n', fp) == EOF)
    return -1;
  return 0;
}

int commit_fprintf_style(struct object *obj, FILE *fp,
                         enum commit_print_style style)
{
  if (!obj || !fp || obj->type != OBJ_COMMIT)
    return -1;

  switch (style) {
  case COMMIT_PRINT_STYLE_DEFAULT:
    return print_default(obj, fp);
  case COMMIT_PRINT_STYLE_RAW:
    return commit_fprintf(obj, fp);
  case COMMIT_PRINT_STYLE_ONELINE:
    return print_oneline(obj, fp);
  default:
    return -1;
  }
}