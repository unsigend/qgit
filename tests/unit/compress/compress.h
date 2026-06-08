#include <compress.h>
#include <stdlib.h>
#include <string.h>
#include <utest.h>

UTEST_CASE(zlib_compress)
{
  enum { LEN = 65536 };
  char *src = malloc(LEN);
  void *dest = NULL;
  size_t destlen = 0;

  EXPECT_NOTNULL(src);
  memset(src, 'a', LEN);

  EXPECT_EQ_INT(zlib_compress(src, LEN, &dest, &destlen), 0);
  EXPECT_NOTNULL(dest);
  EXPECT_LT_UINT(destlen, LEN);

  free(src);
  free(dest);
}

UTEST_CASE(zlib_decompress)
{
  const char *inputs[] = {
      "hello",
      "The quick brown fox jumps over the lazy dog",
  };
  enum { LARGE_LEN = 65536 };
  char *large = malloc(LARGE_LEN);

  EXPECT_NOTNULL(large);
  memset(large, 'x', LARGE_LEN);

  for (size_t i = 0; i < sizeof(inputs) / sizeof(*inputs); i++) {
    size_t srclen = strlen(inputs[i]);
    void *compressed = NULL;
    void *decompressed = NULL;
    size_t compressed_len = 0;
    size_t decompressed_len = 0;

    EXPECT_EQ_INT(
        zlib_compress(inputs[i], srclen, &compressed, &compressed_len), 0);
    EXPECT_EQ_INT(zlib_decompress(compressed, compressed_len, &decompressed,
                                  &decompressed_len),
                  0);
    EXPECT_EQ_UINT(decompressed_len, srclen);
    EXPECT_EQ_INT(memcmp(inputs[i], decompressed, srclen), 0);

    free(compressed);
    free(decompressed);
  }

  {
    void *compressed = NULL;
    void *decompressed = NULL;
    size_t compressed_len = 0;
    size_t decompressed_len = 0;

    EXPECT_EQ_INT(zlib_compress(large, LARGE_LEN, &compressed, &compressed_len),
                  0);
    EXPECT_EQ_INT(zlib_decompress(compressed, compressed_len, &decompressed,
                                  &decompressed_len),
                  0);
    EXPECT_EQ_UINT(decompressed_len, LARGE_LEN);
    EXPECT_EQ_INT(memcmp(large, decompressed, LARGE_LEN), 0);

    free(compressed);
    free(decompressed);
  }

  free(large);
}
