#include <compress.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <utest.h>

static int temp_compress_path(char *path, size_t len)
{
  const char *tmpdir = getenv("TMPDIR");

  if (!tmpdir || !*tmpdir)
    tmpdir = "/tmp";
  if (snprintf(path, len, "%s/qgit-compress-XXXXXX", tmpdir) >= (int)len)
    return -1;

  int fd = mkstemp(path);
  if (fd == -1)
    return -1;
  close(fd);
  return 0;
}

static void temp_compress_unlink(const char *path)
{
  if (path && *path)
    unlink(path);
}

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

UTEST_CASE(zlib_compressf)
{
  const char *inputs[] = {
      "hello",
      "The quick brown fox jumps over the lazy dog",
  };
  enum { LARGE_LEN = 65536 };
  char path[256];
  char *large = malloc(LARGE_LEN);

  EXPECT_NOTNULL(large);
  memset(large, 'x', LARGE_LEN);

  for (size_t i = 0; i < sizeof(inputs) / sizeof(*inputs); i++) {
    size_t srclen = strlen(inputs[i]);

    EXPECT_EQ_INT(temp_compress_path(path, sizeof(path)), 0);
    EXPECT_EQ_INT(zlib_compressf(inputs[i], srclen, path), 0);
    temp_compress_unlink(path);
  }

  EXPECT_EQ_INT(temp_compress_path(path, sizeof(path)), 0);
  EXPECT_EQ_INT(zlib_compressf(large, LARGE_LEN, path), 0);
  temp_compress_unlink(path);

  free(large);
}

UTEST_CASE(zlib_decompressf)
{
  const char *input = "The quick brown fox jumps over the lazy dog";
  char path[256];
  void *dest = NULL;
  size_t destlen = 0;
  size_t srclen = strlen(input);

  EXPECT_EQ_INT(temp_compress_path(path, sizeof(path)), 0);
  EXPECT_EQ_INT(zlib_compressf(input, srclen, path), 0);
  EXPECT_EQ_INT(zlib_decompressf(path, &dest, &destlen), 0);
  EXPECT_NOTNULL(dest);
  EXPECT_EQ_UINT(destlen, srclen);
  EXPECT_EQ_INT(memcmp(input, dest, srclen), 0);
  free(dest);
  temp_compress_unlink(path);

  EXPECT_EQ_INT(temp_compress_path(path, sizeof(path)), 0);
  EXPECT_EQ_INT(zlib_decompressf(path, &dest, &destlen), -1);
  temp_compress_unlink(path);

  EXPECT_EQ_INT(zlib_decompressf("/tmp/qgit-compress-nonexistent", &dest,
                                  &destlen),
                -1);
}
