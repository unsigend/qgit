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

#include <errno.h>
#include <unistd.h>

#include "collection/slist.h"
#include "obj/commit.h"
#include "obj/obj.h"

#define ASCII_COLOR_YELLOW "\033[33m"
#define ASCII_COLOR_RESET "\033[0m"

#define INDENT_WIDTH 4

static int print_default(FILE *stream, struct obj *obj)
{
  int istty = isatty(fileno(stream));
  unsigned char hex[SHA1_HEX_LENGTH];

  if (sha1_to_hex(obj->sha1, hex) == -1)
    return -1;

  const char *zone = obj->commit.azone;
  int sign = (zone[0] == '-') ? -1 : 1;
  int oh = (zone[1] - '0') * 10 + (zone[2] - '0');
  int om = (zone[3] - '0') * 10 + (zone[4] - '0');
  int offset = sign * (oh * 60 * 60 + om * 60);

  time_t t = obj->commit.atime + offset;
  struct tm tm = {0};
  gmtime_r(&t, &tm);
  char datebuf[64];
  strftime(datebuf, sizeof(datebuf), "%a %b %e %H:%M:%S %Y", &tm);

  fprintf(stream, "%scommit %s%s\n", istty ? ASCII_COLOR_YELLOW : "",
          (char *)hex, istty ? ASCII_COLOR_RESET : "");
  if (slist_size(&obj->commit.parents) > 1) {
    fprintf(stream, "Merge: ");
    struct slist_iter iter;
    if (slist_iter_init(&iter, &obj->commit.parents) == -1)
      return -1;
    while (slist_iter_get(&iter)) {
      unsigned char *sha1 = slist_iter_get(&iter);
      if (sha1_to_hex(sha1, hex) == -1)
        return -1;
      fprintf(stream, "%.7s ", (char *)hex);
      slist_iter_inc(&iter);
    }
    fputc('\n', stream);
  }
  fprintf(stream, "Author: %s\n", obj->commit.author);
  fprintf(stream, "Date:   %s %s\n\n", datebuf, zone);
  if (obj->commit.msg) {
    const char *cur = obj->commit.msg;
    fprintf(stream, "%*s", INDENT_WIDTH, " ");
    while (*cur) {
      if (*cur == '\n') {
        fputc('\n', stream);
        if (*(cur + 1)) /* not the last line */
          fprintf(stream, "%*s", INDENT_WIDTH, " ");
      } else
        fputc(*cur, stream);
      cur++;
    }
  }
  return 0;
}

static int print_oneline(FILE *stream, struct obj *obj)
{
  unsigned char hex[SHA1_HEX_LENGTH];
  int istty = isatty(fileno(stream));

  if (sha1_to_hex(obj->sha1, hex) == -1)
    return -1;
  const char *cur = obj->commit.msg;
  fprintf(stream, "%s%.7s %s", istty ? ASCII_COLOR_YELLOW : "", (char *)hex,
          istty ? ASCII_COLOR_RESET : "");
  /* print first line of the message */
  while (cur && *cur != '\n') {
    fputc(*cur, stream);
    cur++;
  }
  fputc('\n', stream);
  return 0;
}

int commit_fprintf_style(FILE *stream, struct obj *obj, commit_style_t style)
{
  if (!stream || !obj) {
    errno = EINVAL;
    return -1;
  }

  switch (style) {
  case COMMIT_STYLE_DEFAULT:
    return print_default(stream, obj);
  case COMMIT_STYLE_ONELINE:
    return print_oneline(stream, obj);
  case COMMIT_STYLE_RAW:
    return commit_fprintf(stream, obj);
  default:
    errno = EINVAL;
    return -1;
  }
}