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

#ifndef COLLECTION_STRING_H
#define COLLECTION_STRING_H

#include <stddef.h>

struct string;
struct string_iter;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Allocate and initialize an empty string with a null terminator.
 *
 * @param str output pointer to receive the new string, must not be NULL
 * @return    0 on success, -1 on failure
 */
extern int string_init(struct string **str);

/**
 * Allocate and initialize a string as a copy of the null-terminated C string
 * src.
 *
 * @param str output pointer to receive the new string, must not be NULL
 * @param src null-terminated source string, must not be NULL
 * @return    0 on success, -1 on failure
 */
extern int string_init_buf(struct string **str, const char *src);

/**
 * Allocate and initialize a string from the first n bytes of src. The result
 * is always null-terminated regardless of whether src contains a null byte
 * within the first n bytes.
 *
 * @param str output pointer to receive the new string, must not be NULL
 * @param src source buffer, must not be NULL
 * @param n   number of bytes to copy
 * @return    0 on success, -1 on failure
 */
extern int string_init_bufn(struct string **str, const char *src, size_t n);

/**
 * Allocate a new string as a deep copy of src.
 *
 * @param dest output pointer to receive the new string, must not be NULL
 * @param src  the string to duplicate, must not be NULL
 * @return     0 on success, -1 on failure
 */
extern int string_dup(struct string **dest, const struct string *src);

/**
 * Allocate a new string as a deep copy of the raw string in str.
 *
 * @param str the string to duplicate, must not be NULL
 * @return    the new string, or NULL on failure
 */
extern char *string_dup_raw(const struct string *str);

/**
 * Release the internal buffer and free the string. NULL-safe.
 *
 * @param str the string to free, or NULL
 */
extern void string_free(struct string *str);

/**
 * Test whether the string contains no characters.
 *
 * @param str the string to inspect, or NULL
 * @return    non-zero if empty or str is NULL, zero otherwise
 */
extern int string_empty(const struct string *str);

/**
 * Return the number of bytes in the string, not counting the null terminator.
 *
 * @param str the string to inspect, or NULL
 * @return    byte length, or 0 if str is NULL
 */
extern size_t string_len(const struct string *str);

/**
 * Return the number of bytes allocated in the internal buffer, not counting
 * the null terminator.
 *
 * @param str the string to inspect, or NULL
 * @return    capacity in bytes, or 0 if str is NULL
 */
extern size_t string_capacity(const struct string *str);

/**
 * Return a pointer to the null-terminated internal character buffer.
 *
 * @param str the string to inspect, or NULL
 * @return    pointer to the buffer, or NULL if str is NULL
 */
extern const char *string_raw(const struct string *str);

/**
 * Return the character at position idx.
 *
 * @param str the string to inspect, or NULL
 * @param idx zero-based character index
 * @return    the character at idx, or '\0' if str is NULL or idx is out of
 *            range
 */
extern char string_at(const struct string *str, size_t idx);

/**
 * Ensure the internal buffer can hold at least cap bytes without
 * reallocation. Does not change the length or content.
 *
 * @param str the string to modify
 * @param cap minimum capacity in bytes, not counting the null terminator
 * @return    0 on success, -1 on failure
 */
extern int string_reserve(struct string *str, size_t cap);

/**
 * Reallocate the internal buffer so its capacity matches the current length,
 * releasing unused memory.
 *
 * @param str the string to shrink
 * @return    0 on success, -1 on failure
 */
extern int string_shrink(struct string *str);

/**
 * Reset the string to empty without releasing the internal buffer.
 *
 * @param str the string to clear, or NULL
 */
extern void string_clear(struct string *str);

/**
 * Replace the string content with a copy of the null-terminated C string src.
 *
 * @param str the string to modify
 * @param src null-terminated replacement, must not be NULL
 * @return    0 on success, -1 on failure
 */
extern int string_assign(struct string *str, const char *src);

/**
 * Replace the string content with the first n bytes of src.
 *
 * @param str the string to modify
 * @param src source buffer, must not be NULL
 * @param n   number of bytes to copy
 * @return    0 on success, -1 on failure
 */
extern int string_assignn(struct string *str, const char *src, size_t n);

/**
 * Replace the content of dest with a copy of src.
 *
 * @param dest the string to overwrite
 * @param src  the string to copy from, must not be NULL
 * @return     0 on success, -1 on failure
 */
extern int string_copy(struct string *dest, const struct string *src);

/**
 * Append the single character c to the end of the string.
 *
 * @param str the string to modify
 * @param c   character to append
 * @return    0 on success, -1 on failure
 */
extern int string_pushback(struct string *str, char c);

/**
 * Remove the last character. When dest is non-NULL, store the character there
 * otherwise discard it.
 *
 * @param str  the string to modify
 * @param dest pointer to receive the removed character, or NULL to discard
 * @return     0 on success, -1 if str is NULL or the string is empty
 */
extern int string_popback(struct string *str, char *dest);

/**
 * Append the null-terminated C string src to the end of str.
 *
 * @param str the string to modify
 * @param src null-terminated string to append, must not be NULL
 * @return    0 on success, -1 on failure
 */
extern int string_append(struct string *str, const char *src);

/**
 * Append the first n bytes of src to the end of str.
 *
 * @param str the string to modify
 * @param src source buffer, must not be NULL
 * @param n   number of bytes to append
 * @return    0 on success, -1 on failure
 */
extern int string_appendn(struct string *str, const char *src, size_t n);

/**
 * Append the content of other to the end of str.
 *
 * @param str   the string to modify
 * @param other the string to append, must not be NULL
 * @return      0 on success, -1 on failure
 */
extern int string_append_str(struct string *str, const struct string *other);

/**
 * Prepend the null-terminated C string src to the beginning of str.
 *
 * @param str the string to modify
 * @param src null-terminated string to prepend, must not be NULL
 * @return    0 on success, -1 on failure
 */
extern int string_prepend(struct string *str, const char *src);

/**
 * Prepend the first n bytes of src to the beginning of str.
 *
 * @param str the string to modify
 * @param src source buffer, must not be NULL
 * @param n   number of bytes to prepend
 * @return    0 on success, -1 on failure
 */
extern int string_prependn(struct string *str, const char *src, size_t n);

/**
 * Insert the null-terminated C string src before position pos, shifting the
 * tail right. When pos is at or beyond the current length, behaves like
 * string_append.
 *
 * @param str the string to modify
 * @param pos zero-based insertion position
 * @param src null-terminated string to insert, must not be NULL
 * @return    0 on success, -1 on failure
 */
extern int string_insert(struct string *str, size_t pos, const char *src);

/**
 * Remove n bytes starting at position pos, shifting the tail left. Clamps to
 * the available length when pos + n would exceed it.
 *
 * @param str the string to modify
 * @param pos zero-based start position of the range to remove
 * @param n   number of bytes to remove
 * @return    0 on success, -1 if str is NULL or pos is out of range
 */
extern int string_erase(struct string *str, size_t pos, size_t n);

/**
 * Find the first occurrence of the null-terminated substr in str.
 *
 * @param str    the string to search, or NULL
 * @param substr null-terminated substring to look for, must not be NULL
 * @return       pointer to the first match in the internal buffer, or NULL if
 *               not found or str is NULL
 */
extern const char *string_find(const struct string *str, const char *substr);

/**
 * Find the last occurrence of the null-terminated substr in str.
 *
 * @param str    the string to search, or NULL
 * @param substr null-terminated substring to look for, must not be NULL
 * @return       pointer to the last match in the internal buffer, or NULL if
 *               not found or str is NULL
 */
extern const char *string_find_last(const struct string *str,
                                    const char *substr);

/**
 * Test whether the null-terminated substr appears anywhere in str.
 *
 * @param str    the string to search, or NULL
 * @param substr null-terminated substring to look for, must not be NULL
 * @return       non-zero if found, zero otherwise or if str is NULL
 */
extern int string_contains(const struct string *str, const char *substr);

/**
 * Test whether str begins with the null-terminated prefix.
 *
 * @param str    the string to inspect, or NULL
 * @param prefix null-terminated prefix to match, must not be NULL
 * @return       non-zero if str starts with prefix, zero otherwise or if str
 *               is NULL
 */
extern int string_starts_with(const struct string *str, const char *prefix);

/**
 * Test whether str ends with the null-terminated suffix.
 *
 * @param str    the string to inspect, or NULL
 * @param suffix null-terminated suffix to match, must not be NULL
 * @return       non-zero if str ends with suffix, zero otherwise or if str is
 *               NULL
 */
extern int string_ends_with(const struct string *str, const char *suffix);

/**
 * Test whether two strings have identical content.
 *
 * @param a the first string, or NULL
 * @param b the second string, or NULL
 * @return  non-zero if equal, zero otherwise; two NULL pointers compare equal
 */
extern int string_eq(const struct string *a, const struct string *b);

/**
 * Test whether the string content equals the null-terminated C string buf.
 *
 * @param str the string to inspect, or NULL
 * @param buf null-terminated C string to compare against, must not be NULL
 * @return    non-zero if equal, zero otherwise or if str is NULL
 */
extern int string_eq_buf(const struct string *str, const char *buf);

/**
 * Compare two strings lexicographically, equivalent to strcmp on their raw
 * buffers.
 *
 * @param a the first string, or NULL
 * @param b the second string, or NULL
 * @return  negative if a < b, zero if equal, positive if a > b; NULL sorts
 *          before non-NULL
 */
extern int string_cmp(const struct string *a, const struct string *b);

/**
 * Convert all ASCII uppercase letters in the string to lowercase in-place.
 *
 * @param str the string to modify, or NULL
 */
extern void string_lowercase(struct string *str);

/**
 * Convert all ASCII lowercase letters in the string to uppercase in-place.
 *
 * @param str the string to modify, or NULL
 */
extern void string_uppercase(struct string *str);

/**
 * Remove leading ASCII whitespace from the string in-place.
 *
 * @param str the string to modify, or NULL
 */
extern void string_trim_start(struct string *str);

/**
 * Remove trailing ASCII whitespace from the string in-place.
 *
 * @param str the string to modify, or NULL
 */
extern void string_trim_end(struct string *str);

/**
 * Remove both leading and trailing ASCII whitespace from the string in-place.
 *
 * @param str the string to modify, or NULL
 */
extern void string_trim(struct string *str);

/**
 * Reverse the string in-place.
 *
 * @param str the string to modify, or NULL
 */
extern void string_reverse(struct string *str);

/**
 * Replace the first occurrence of the null-terminated pattern with the
 * null-terminated replacement. The string may grow or shrink.
 *
 * @param str         the string to modify
 * @param pattern     null-terminated substring to find, must not be NULL
 * @param replacement null-terminated replacement, must not be NULL
 * @return            0 on success or pattern not found, -1 on allocation
 *                    failure
 */
extern int string_replace(struct string *str, const char *pattern,
                          const char *replacement);

/**
 * Replace every occurrence of the null-terminated pattern with the
 * null-terminated replacement.
 *
 * @param str         the string to modify
 * @param pattern     null-terminated substring to find, must not be NULL
 * @param replacement null-terminated replacement, must not be NULL
 * @return            0 on success or pattern not found, -1 on allocation
 *                    failure
 */
extern int string_replace_all(struct string *str, const char *pattern,
                              const char *replacement);

/**
 * Allocate a new string containing the substring of str starting at pos with
 * length n. Clamps n to the available length when pos + n would exceed it.
 *
 * @param out output pointer to receive the new string, must not be NULL
 * @param str the source string
 * @param pos zero-based start position
 * @param n   number of bytes to extract
 * @return    0 on success, -1 on failure or if pos is out of range
 */
extern int string_substr(struct string **out, const struct string *str,
                         size_t pos, size_t n);

/**
 * Split str on every occurrence of the null-terminated separator sep, storing
 * each part as a newly allocated string in *parts. The caller frees the result
 * with string_split_free.
 *
 * @param parts output pointer to receive the array of strings, must not be NULL
 * @param count output pointer to receive the number of parts, must not be NULL
 * @param str   the string to split
 * @param sep   null-terminated separator, must not be NULL and must be
 *              non-empty
 * @return      0 on success, -1 on failure
 */
extern int string_split(struct string ***parts, size_t *count,
                        const struct string *str, const char *sep);

/**
 * Free an array of strings returned by string_split.
 *
 * @param parts the array to free, or NULL
 * @param count number of elements in parts
 */
extern void string_split_free(struct string **parts, size_t count);

/**
 * Allocate a new string by joining n null-terminated C string arguments with
 * sep between each pair. The variadic arguments must all be const char *.
 *
 * @param out output pointer to receive the new string, must not be NULL
 * @param sep null-terminated separator inserted between parts, or NULL for no
 *            separator
 * @param n   number of variadic arguments
 * @param ... n const char * arguments to join
 * @return    0 on success, -1 on failure
 */
extern int string_join(struct string **out, const char *sep, size_t n, ...);

/**
 * Allocate a new string by joining an array of n strings with sep between each
 * pair.
 *
 * @param out   output pointer to receive the new string, must not be NULL
 * @param sep   null-terminated separator, or NULL for no separator
 * @param parts array of strings to join, must not be NULL
 * @param n     number of elements in parts
 * @return      0 on success, -1 on failure
 */
extern int string_join_str(struct string **out, const char *sep,
                           const struct string **parts, size_t n);

/**
 * Allocate and initialize an iterator positioned at the first character of
 * str.
 *
 * @param iter output pointer to receive the new iterator, must not be NULL
 * @param str  the string to traverse, must not be NULL
 * @return     0 on success, -1 on failure
 */
extern int string_iter_init(struct string_iter **iter, struct string *str);

/**
 * Advance the iterator forward by one character.
 *
 * @param iter the iterator to advance, or NULL
 */
extern void string_iter_inc(struct string_iter *iter);

/**
 * Move the iterator backward by one character.
 *
 * @param iter the iterator to move, or NULL
 */
extern void string_iter_dec(struct string_iter *iter);

/**
 * Return the character at the current iterator position.
 *
 * @param iter the iterator to read from, or NULL
 * @return     the current character, or '\0' if iter is NULL or out of range
 */
extern char string_iter_get(struct string_iter *iter);

/**
 * Free the iterator.
 *
 * @param iter the iterator to free, or NULL
 */
extern void string_iter_free(struct string_iter *iter);

/*
 * Test whether the string starts with the null-terminated prefix.
 *
 * @param str    the string to inspect, or NULL
 * @param prefix null-terminated prefix to match, must not be NULL
 * @return       non-zero if str starts with prefix, zero otherwise or if str
 *               is NULL
 */
extern int str_startswith(const char *str, const char *prefix);

/**
 * Test whether the string ends with the null-terminated suffix.
 *
 * @param str    the string to inspect, or NULL
 * @param suffix null-terminated suffix to match, must not be NULL
 * @return       non-zero if str ends with suffix, zero otherwise or if str is
 *               NULL
 */
extern int str_endswith(const char *str, const char *suffix);

#ifdef __cplusplus
}
#endif

#endif
