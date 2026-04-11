#include <compress.h>
#include <stdlib.h>
#include <string.h>
#include <utest.h>

#define EXPECT_ROUND_TRIP(src, len)                                            \
  do {                                                                         \
    unsigned char *comp_rt = NULL;                                             \
    size_t complen_rt = 0;                                                     \
    unsigned char *out_rt = NULL;                                              \
    size_t outlen_rt = 0;                                                      \
    EXPECT_EQ_INT(zlib_compress((src), (len), &comp_rt, &complen_rt), 0);      \
    EXPECT_NOTNULL(comp_rt);                                                   \
    EXPECT_EQ_INT(                                                             \
        zlib_decompress(comp_rt, complen_rt, &out_rt, &outlen_rt), 0);         \
    EXPECT_NOTNULL(out_rt);                                                    \
    EXPECT_EQ_UINT(outlen_rt, (uint64_t)(len));                                \
    EXPECT_EQ_INT(memcmp((src), out_rt, (len)), 0);                            \
    free(comp_rt);                                                             \
    free(out_rt);                                                              \
  } while (0)

UTEST_CASE(zlib_round_trip_empty)
{
  static const unsigned char src[] = "";
  EXPECT_ROUND_TRIP(src, 0);
}

UTEST_CASE(zlib_round_trip_short)
{
  static const unsigned char src[] = "hello world";
  EXPECT_ROUND_TRIP(src, sizeof src - 1);
}

UTEST_CASE(zlib_round_trip_zeros_large)
{
  unsigned char src[100000];
  memset(src, 0, sizeof src);
  EXPECT_ROUND_TRIP(src, sizeof src);
}

UTEST_CASE(zlib_round_trip_pattern)
{
  unsigned char src[4096];
  size_t i;
  unsigned x = 12345u;
  for (i = 0; i < sizeof src; i++) {
    x = x * 1103515245u + 12345u;
    src[i] = (unsigned char)(x >> 16);
  }
  EXPECT_ROUND_TRIP(src, sizeof src);
}

UTEST_CASE(zlib_round_trip_two_payloads)
{
  static const unsigned char a[] = {0, 1, 2, 3, 4, 5};
  static const unsigned char b[] = "second buffer payload";
  EXPECT_ROUND_TRIP(a, sizeof a);
  EXPECT_ROUND_TRIP(b, sizeof b - 1);
}

UTEST_CASE(zlib_decompress_invalid)
{
  static const unsigned char junk[] = {0x78, 0xda, 0xff, 0xff, 0xff};
  unsigned char *out = NULL;
  size_t outlen = 0;

  EXPECT_EQ_INT(zlib_decompress(junk, sizeof junk, &out, &outlen), -1);
  EXPECT_NULL(out);
}

UTEST_CASE(zlib_round_trip_single_byte)
{
  static const unsigned char src[] = {0xab};
  EXPECT_ROUND_TRIP(src, 1);
}

UTEST_CASE(zlib_round_trip_byte_spectrum)
{
  unsigned char src[256];
  size_t i;
  for (i = 0; i < 256; i++)
    src[i] = (unsigned char)i;
  EXPECT_ROUND_TRIP(src, sizeof src);
}

UTEST_CASE(zlib_round_trip_utf8)
{
  static const unsigned char src[] = "caf\xe9 \xe2\x98\x83 snowman";
  EXPECT_ROUND_TRIP(src, sizeof src - 1);
}

UTEST_CASE(zlib_decompress_empty_source)
{
  static const unsigned char empty[] = "";
  unsigned char *out = NULL;
  size_t outlen = 0;

  EXPECT_EQ_INT(zlib_decompress(empty, 0, &out, &outlen), -1);
  EXPECT_NULL(out);
}

UTEST_CASE(zlib_decompress_truncated)
{
  static const unsigned char src[] = "payload for truncation";
  unsigned char *comp = NULL;
  size_t complen = 0;
  unsigned char *out = NULL;
  size_t outlen = 0;

  EXPECT_EQ_INT(zlib_compress(src, sizeof src - 1, &comp, &complen), 0);
  EXPECT_NOTNULL(comp);
  EXPECT_GT_UINT(complen, 2u);
  EXPECT_EQ_INT(zlib_decompress(comp, complen - 1, &out, &outlen), -1);
  EXPECT_NULL(out);
  free(comp);
}

UTEST_CASE(zlib_compress_deterministic)
{
  static const unsigned char src[] = "same input twice";
  unsigned char *c1 = NULL;
  size_t l1 = 0;
  unsigned char *c2 = NULL;
  size_t l2 = 0;

  EXPECT_EQ_INT(zlib_compress(src, sizeof src - 1, &c1, &l1), 0);
  EXPECT_EQ_INT(zlib_compress(src, sizeof src - 1, &c2, &l2), 0);
  EXPECT_NOTNULL(c1);
  EXPECT_NOTNULL(c2);
  EXPECT_EQ_UINT((uint64_t)l1, (uint64_t)l2);
  EXPECT_EQ_INT(memcmp(c1, c2, l1), 0);
  free(c1);
  free(c2);
}
