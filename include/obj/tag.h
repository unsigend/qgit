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

#ifndef OBJ_TAG_H
#define OBJ_TAG_H

#include <stdio.h>

#include "obj/sign.h"
#include "sha1.h"

struct obj;

/* Raw payload format for annotated tag:
     object <sha1-40>\n
     type <commit|tree|blob|tag>\n
     tag <name>\n
     tagger <name> <email> <timestamp> <timezone>\n
     \n
     <message>
*/
struct tag {
  unsigned char object[SHA1_DIGLEN];
  const char *type;
  const char *name;
  struct sign tagger;
  const char *msg;
};

extern int tag_parse(struct obj *obj);
extern void tag_close(struct tag *tag);
extern int tag_fprintf(struct obj *obj, FILE *fp);

extern struct obj *tag_create(const struct tag *tag);

#endif