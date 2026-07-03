/* collection - A generic data structure and algorithms library
 * Copyright (C) 2025 Yixiang Qiu
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

#include <collection/string.h>
#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define GROWTH_FACTOR 2
#define INITIAL_CAPACITY 16

struct string {
    char *buf;
    size_t len;
    size_t cap;
};

struct string_iter {
    const struct string *str;
    size_t idx;
};

static char *strrstr(const char *haystack, const char *needle);

int string_init(struct string **str) { return string_init_bufn(str, NULL, 0); }

int string_init_buf(struct string **str, const char *src)
{
    if (!str || !src)
        return -1;
    return string_init_bufn(str, src, strlen(src));
}

int string_init_bufn(struct string **str, const char *src, size_t n)
{
    if (!str || (n && !src))
        return -1;
    struct string *s = calloc(1, sizeof(struct string));
    if (!s)
        return -1;
    s->buf = malloc(n + 1);
    if (!s->buf) {
        string_free(s);
        return -1;
    }
    if (n && src)
        memcpy(s->buf, src, n);
    s->buf[n] = '\0';
    s->len = n;
    s->cap = n;
    *str = s;
    return 0;
}

int string_dup(struct string **dest, const struct string *src)
{
    if (!dest || !src)
        return -1;
    return string_init_bufn(dest, src->buf, src->len);
}

void string_free(struct string *str)
{
    if (!str)
        return;
    if (str->buf)
        free(str->buf);
    free(str);
}

int string_empty(const struct string *str)
{
    if (!str)
        return 1;
    return str->len == 0;
}

size_t string_len(const struct string *str)
{
    if (!str)
        return 0;
    return str->len;
}

size_t string_capacity(const struct string *str)
{
    if (!str)
        return 0;
    return str->cap;
}

const char *string_raw(const struct string *str)
{
    if (!str)
        return NULL;
    return str->buf;
}

char string_at(const struct string *str, size_t idx)
{
    if (!str)
        return '\0';
    if (idx >= str->len) {
        errno = EINVAL;
        return '\0';
    }
    return str->buf[idx];
}

int string_reserve(struct string *str, size_t cap)
{
    if (!str)
        return -1;
    if (str->cap >= cap)
        return 0;
    char *newbuf = realloc(str->buf, cap + 1);
    if (!newbuf)
        return -1;
    newbuf[str->len] = '\0';
    str->buf = newbuf;
    str->cap = cap;
    return 0;
}

int string_shrink(struct string *str)
{
    if (!str)
        return -1;
    if (str->len == str->cap)
        return 0;
    char *newbuf = realloc(str->buf, str->len + 1);
    if (!newbuf)
        return -1;
    newbuf[str->len] = '\0';
    str->buf = newbuf;
    str->cap = str->len;
    return 0;
}

void string_clear(struct string *str)
{
    if (!str)
        return;

    str->len = 0;
    str->buf[0] = '\0';
}

int string_assign(struct string *str, const char *src)
{
    if (!str || !src)
        return -1;
    return string_assignn(str, src, strlen(src));
}

int string_assignn(struct string *str, const char *src, size_t n)
{
    if (!str || !src)
        return -1;

    if (n <= str->cap) {
        if (n)
            memcpy(str->buf, src, n);
        str->buf[n] = '\0';
        str->len = n;
        return 0;
    }

    char *newbuf = realloc(str->buf, n + 1);
    if (!newbuf)
        return -1;
    if (n)
        memcpy(newbuf, src, n);
    newbuf[n] = '\0';
    str->buf = newbuf;
    str->cap = n;
    str->len = n;
    return 0;
}

int string_copy(struct string *dest, const struct string *src)
{
    if (!dest || !src)
        return -1;
    return string_assignn(dest, src->buf, src->len);
}

int string_pushback(struct string *str, char c)
{
    if (!str)
        return -1;
    if (str->len == str->cap) {
        int err = string_reserve(str, str->cap ? str->cap * GROWTH_FACTOR
                                               : INITIAL_CAPACITY);
        if (err)
            return err;
    }
    str->buf[str->len] = c;
    str->buf[++str->len] = '\0';
    return 0;
}

int string_popback(struct string *str, char *dest)
{
    if (!str)
        return -1;
    if (str->len == 0) {
        errno = EINVAL;
        return -1;
    }
    if (dest)
        *dest = str->buf[str->len - 1];
    str->buf[--str->len] = '\0';
    return 0;
}

int string_append(struct string *str, const char *src)
{
    if (!str || !src)
        return -1;
    return string_appendn(str, src, strlen(src));
}

int string_appendn(struct string *str, const char *src, size_t n)
{
    if (!str || !src)
        return -1;
    if (!n)
        return 0;
    if (n > str->cap - str->len) {
        int err = string_reserve(str, str->len + n);
        if (err)
            return err;
    }
    memcpy(str->buf + str->len, src, n);
    str->len += n;
    str->buf[str->len] = '\0';
    return 0;
}

int string_append_str(struct string *str, const struct string *other)
{
    if (!str || !other)
        return -1;
    return string_appendn(str, other->buf, other->len);
}

int string_prepend(struct string *str, const char *src)
{
    if (!str || !src)
        return -1;
    return string_prependn(str, src, strlen(src));
}

int string_prependn(struct string *str, const char *src, size_t n)
{
    if (!str || !src)
        return -1;
    if (!n)
        return 0;
    if (n > str->cap - str->len) {
        int err = string_reserve(str, str->len + n);
        if (err)
            return err;
    }
    memmove(str->buf + n, str->buf, str->len);
    memcpy(str->buf, src, n);
    str->len += n;
    str->buf[str->len] = '\0';
    return 0;
}

int string_insert(struct string *str, size_t pos, const char *src)
{
    if (!str || !src)
        return -1;
    size_t n = strlen(src);
    if (!n)
        return 0;

    if (pos >= str->len)
        return string_append(str, src);
    if (pos == 0)
        return string_prepend(str, src);

    if (n > str->cap - str->len) {
        int err = string_reserve(str, str->len + n);
        if (err)
            return err;
    }
    memmove(str->buf + pos + n, str->buf + pos, str->len - pos);
    memcpy(str->buf + pos, src, n);
    str->len += n;
    str->buf[str->len] = '\0';
    return 0;
}

int string_erase(struct string *str, size_t pos, size_t n)
{
    if (!str)
        return -1;
    if (pos > str->len) {
        errno = EINVAL;
        return -1;
    }
    if (!n)
        return 0;
    if (pos + n >= str->len)
        n = str->len - pos;

    memmove(str->buf + pos, str->buf + pos + n, str->len - pos - n);
    str->len -= n;
    str->buf[str->len] = '\0';
    return 0;
}

const char *string_find(const struct string *str, const char *substr)
{
    if (!str || !substr || !strlen(substr))
        return NULL;
    return strstr(str->buf, substr);
}

const char *string_find_last(const struct string *str, const char *substr)
{
    if (!str || !substr || !strlen(substr))
        return NULL;
    return strrstr(str->buf, substr);
}

int string_contains(const struct string *str, const char *substr)
{
    if (!str || !substr || !strlen(substr))
        return 0;
    return string_find(str, substr) != NULL;
}

int string_starts_with(const struct string *str, const char *prefix)
{
    if (!str || !prefix)
        return 0;
    size_t len = string_len(str);
    size_t prefixlen = strlen(prefix);
    return prefixlen <= len && memcmp(str->buf, prefix, prefixlen) == 0;
}

int string_ends_with(const struct string *str, const char *suffix)
{
    if (!str || !suffix)
        return 0;
    size_t len = string_len(str);
    size_t suffixlen = strlen(suffix);
    return suffixlen <= len &&
           memcmp(str->buf + len - suffixlen, suffix, suffixlen) == 0;
}

int string_eq(const struct string *a, const struct string *b)
{
    if (!a && !b)
        return 1;
    if (!a || !b)
        return 0;
    return a->len == b->len && memcmp(a->buf, b->buf, a->len) == 0;
}

int string_eq_buf(const struct string *str, const char *buf)
{
    if (!str || !buf)
        return 0;
    return str->len == strlen(buf) && memcmp(str->buf, buf, str->len) == 0;
}

int string_cmp(const struct string *a, const struct string *b)
{
    if (!a && !b)
        return 0;
    if (!a)
        return -1;
    if (!b)
        return 1;
    int r = memcmp(a->buf, b->buf, a->len < b->len ? a->len : b->len);
    return r ? r : (a->len > b->len) - (a->len < b->len);
}

void string_lowercase(struct string *str)
{
    if (!str)
        return;
    for (size_t i = 0; i < str->len; i++)
        str->buf[i] = tolower((unsigned char)str->buf[i]);
}

void string_uppercase(struct string *str)
{
    if (!str)
        return;
    for (size_t i = 0; i < str->len; i++)
        str->buf[i] = toupper((unsigned char)str->buf[i]);
}

void string_trim_start(struct string *str)
{
    if (!str)
        return;
    size_t i = 0;
    while (i < str->len && isspace((unsigned char)str->buf[i]))
        i++;
    if (i > 0) {
        memmove(str->buf, str->buf + i, str->len - i);
        str->len -= i;
        str->buf[str->len] = '\0';
    }
}

void string_trim_end(struct string *str)
{
    if (!str)
        return;
    size_t i = str->len;
    while (i > 0 && isspace((unsigned char)str->buf[i - 1]))
        i--;
    if (i < str->len) {
        str->buf[i] = '\0';
        str->len = i;
    }
}

void string_trim(struct string *str)
{
    if (!str)
        return;
    string_trim_start(str);
    string_trim_end(str);
}

void string_reverse(struct string *str)
{
    if (!str)
        return;
    if (!str->len || str->len == 1)
        return;
    size_t i = 0;
    size_t j = str->len - 1;
    while (i < j) {
        char tmp = str->buf[i];
        str->buf[i] = str->buf[j];
        str->buf[j] = tmp;
        i++;
        j--;
    }
}

int string_replace(struct string *str, const char *pattern,
                   const char *replacement)
{
    if (!str || !pattern || !replacement)
        return -1;
    size_t patternlen = strlen(pattern);
    if (!patternlen)
        return 0;
    size_t replacelen = strlen(replacement);

    const char *pos = string_find(str, pattern);
    if (!pos)
        return 0;

    size_t posoff = pos - str->buf;
    if (replacelen > patternlen) {
        int err = string_reserve(str, str->len + (replacelen - patternlen));
        if (err)
            return err;
    }
    memmove(str->buf + posoff + replacelen, str->buf + posoff + patternlen,
            str->len - posoff - patternlen);
    memcpy(str->buf + posoff, replacement, replacelen);
    str->len = str->len - patternlen + replacelen;
    str->buf[str->len] = '\0';
    return 0;
}

int string_replace_all(struct string *str, const char *pattern,
                       const char *replacement)
{
    if (!str || !pattern || !replacement)
        return -1;
    size_t patternlen = strlen(pattern);
    if (!patternlen)
        return 0;
    size_t replacelen = strlen(replacement);

    size_t off = 0;
    while (off <= str->len) {
        const char *pos = strstr(str->buf + off, pattern);
        if (!pos)
            break;
        size_t posoff = pos - str->buf;
        if (replacelen > patternlen) {
            int err = string_reserve(str, str->len + (replacelen - patternlen));
            if (err)
                return err;
        }
        memmove(str->buf + posoff + replacelen, str->buf + posoff + patternlen,
                str->len - posoff - patternlen);
        memcpy(str->buf + posoff, replacement, replacelen);
        str->len = str->len - patternlen + replacelen;
        str->buf[str->len] = '\0';
        off = posoff + replacelen;
    }
    return 0;
}

int string_substr(struct string **out, const struct string *str, size_t pos,
                  size_t n)
{
    if (!out || !str)
        return -1;
    if (pos > str->len) {
        errno = EINVAL;
        return -1;
    }

    if (pos + n > str->len)
        n = str->len - pos;

    return string_init_bufn(out, str->buf + pos, n);
}

int string_split(struct string ***parts, size_t *count,
                 const struct string *str, const char *sep)
{
    if (!parts || !count || !str || !sep)
        return -1;

    *parts = NULL;
    *count = 0;

    size_t seplen = strlen(sep);
    if (!seplen)
        return -1;

    const char *cur = str->buf;
    const char *end = str->buf + str->len;

    while (cur <= end) {
        const char *next = strstr(cur, sep);
        if (!next)
            next = end;

        struct string *part = NULL;
        if (string_init_bufn(&part, cur, (size_t)(next - cur)) != 0)
            goto fail;

        struct string **tmp = realloc(*parts, (*count + 1) * sizeof(*tmp));
        if (!tmp) {
            string_free(part);
            goto fail;
        }
        *parts = tmp;
        (*parts)[(*count)++] = part;

        if (next == end)
            break;
        cur = next + seplen;
    }
    return 0;

fail:
    string_split_free(*parts, *count);
    *parts = NULL;
    *count = 0;
    return -1;
}

void string_split_free(struct string **parts, size_t count)
{
    if (!parts)
        return;
    for (size_t i = 0; i < count; i++)
        string_free(parts[i]);
    free(parts);
}

int string_join(struct string **out, const char *sep, size_t n, ...)
{
    if (!out)
        return -1;
    if (!n)
        return string_init_buf(out, "");
    va_list args;
    va_start(args, n);

    struct string *str = NULL;
    int err = string_init(&str);
    if (err)
        return err;

    int first = 1;
    for (size_t i = 0; i < n; i++) {
        const char *arg = va_arg(args, const char *);
        if (!arg)
            continue;
        if (!first && sep) {
            err = string_append(str, sep); /* append separator */
            if (err)
                goto fail;
        }
        err = string_append(str, arg); /* append argument */
        if (err)
            goto fail;
        first = 0;
    }
    va_end(args);
    *out = str;
    return 0;
fail:
    string_free(str);
    va_end(args);
    return -1;
}

int string_join_str(struct string **out, const char *sep,
                    const struct string **parts, size_t n)
{
    if (!out || !parts)
        return -1;
    if (!n)
        return string_init_buf(out, "");

    struct string *str = NULL;
    int err = string_init(&str);
    if (err)
        return err;

    int first = 1;
    for (size_t i = 0; i < n; i++) {
        const struct string *part = parts[i];
        if (!part)
            continue;
        if (!first && sep) {
            err = string_append(str, sep); /* append separator */
            if (err)
                goto fail;
        }
        err = string_append_str(str, part); /* append string */
        if (err)
            goto fail;
        first = 0;
    }
    *out = str;
    return 0;
fail:
    string_free(str);
    return -1;
}

static char *strrstr(const char *haystack, const char *needle)
{
    if (!haystack || !needle)
        return NULL;

    size_t hlen = strlen(haystack);
    size_t nlen = strlen(needle);

    if (nlen == 0)
        return (char *)haystack + hlen;
    if (nlen > hlen)
        return NULL;

    for (size_t i = hlen - nlen;; i--) {
        if (memcmp(haystack + i, needle, nlen) == 0)
            return (char *)haystack + i;
        if (i == 0)
            break;
    }

    return NULL;
}

int string_iter_init(struct string_iter **iter, struct string *str)
{
    if (!iter || !str)
        return -1;
    struct string_iter *it = malloc(sizeof(struct string_iter));
    if (!it)
        return -1;
    it->str = str;
    it->idx = 0;
    *iter = it;
    return 0;
}

void string_iter_inc(struct string_iter *iter)
{
    if (!iter || iter->idx >= iter->str->len)
        return;
    iter->idx++;
}

void string_iter_dec(struct string_iter *iter)
{
    if (!iter || iter->idx == 0)
        return;
    iter->idx--;
}

char string_iter_get(struct string_iter *iter)
{
    if (!iter || iter->idx >= iter->str->len)
        return '\0';
    return iter->str->buf[iter->idx];
}

void string_iter_free(struct string_iter *iter)
{
    free(iter);
}