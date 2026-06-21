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
#include <fcntl.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "argparse.h"
#include "die.h"
#include "obj/object.h"

static void map_buf(const char *path, void **buf, size_t *buflen)
{
  int fd = -1;
  struct stat st;

  if ((fd = open(path, O_RDONLY)) == -1)
    die_errno();
  if (fstat(fd, &st) == -1)
    die_errno();
  *buflen = st.st_size;
  if (*buflen == 0) {
    close(fd);
    *buf = NULL;
    return;
  }

  if ((*buf = mmap(NULL, *buflen, PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED)
    die_errno();
  close(fd);
}

int cmd_hash_object(int argc, char **argv)
{
  int w = 0;
  const char *type = "blob";
  const char *path = NULL;

  struct argparse ctx;
  struct argparse_opt opts[] = {
      OPT_HELP(),
      OPT_STR('t', "type", "specify the type of the object", &type,
              OPT_REQUIRED),
      OPT_BOOL('w', "write", "write the object to the repository", &w),
      OPT_END(),
  };

  static const char *usages[] = {
      "qgit hash-object [-t <type>] [-w | --write] <file>",
  };

  struct argparse_desc desc = {
      .prog = "qgit hash-object",
      .desc = "Compute object ID and optionally create a blob from a file",
      .usages = usages,
      .nusages = sizeof(usages) / sizeof(usages[0]),
  };

  if (argparse_init(&ctx, opts, &desc) == -1)
    die("%s", argparse_strerror(&ctx));
  if (argparse_parse(&ctx, argc, argv) == -1)
    die("%s", argparse_strerror(&ctx));
  if (argparse_getremargc(&ctx) == 0)
    die("missing <file>");
  path = argparse_getremargv(&ctx)[0];

  struct repo *repo = NULL;
  struct obj *obj = NULL;
  void *buf = NULL;
  size_t buflen = 0;
  unsigned char hex[SHA1_HEXLEN];

  map_buf(path, &buf, &buflen);
  if (!(obj = obj_open_buf(buf, buflen, obj_type_from_str(type))))
    die_errno();
  if (buf)
    munmap(buf, buflen);

  if (obj_write_buf(obj, &buf, &buflen) == -1)
    die_errno();
  if (sha1(buf, buflen, obj->sha1) == -1)
    die_errno();
  if (sha1_to_hex(obj->sha1, hex) == -1)
    die_errno();
  printf("%s\n", hex);

  if (w) {
    if (!(repo = repo_findcwd()))
      die_errno();
    if (obj_store(repo, obj->sha1, buf, buflen) == -1)
      die_errno();
    repo_close(repo);
  }

  free(buf);
  obj_close(obj);
  argparse_fini(&ctx);
  return 0;
}