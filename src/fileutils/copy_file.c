/* miniutils - A minimal GNU coreutils implementation
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

#include <fcntl.h>
#include <fileutil.h>
#include <sys/stat.h>
#include <unistd.h>

int copy_file(const char *dest, const char *src)
{
    int srcfd = -1, dstfd = -1;
    struct stat st;
    mode_t mode;
    char buf[4096];
    ssize_t n;

    if (stat(src, &st) == -1)
        return -1;
    mode = st.st_mode;

    if ((srcfd = open(src, O_RDONLY)) == -1)
        return -1;

    if ((dstfd = open(dest, O_WRONLY | O_CREAT | O_TRUNC, mode)) == -1) {
        close(srcfd);
        return -1;
    }

    while (1) {
        n = read_all(srcfd, buf, sizeof(buf));
        if (n == -1) {
            close(srcfd);
            close(dstfd);
            unlink(dest);
            return -1;
        }
        if (n == 0)
            break;
        if (write_all(dstfd, buf, n) == -1) {
            close(srcfd);
            close(dstfd);
            unlink(dest);
            return -1;
        }
    }

    if (close(srcfd) == -1) {
        close(dstfd);
        return -1;
    }
    if (close(dstfd) == -1)
        return -1;

    return 0;
}
