#include <errno.h>
#include <sha1.h>
#include <stdlib.h>
#include <string.h>
#include <utest.h>

static int digest_nonzero(const unsigned char *digest)
{
  for (size_t i = 0; i < SHA1_DIGLEN; i++) {
    if (digest[i] != 0)
      return 1;
  }
  return 0;
}

#define EXPECT_HEX_ROUNDTRIP(digest)                                           \
  do {                                                                         \
    unsigned char HEX[SHA1_HEXLEN];                                            \
    unsigned char BACK[SHA1_DIGLEN];                                           \
    EXPECT_EQ_INT(sha1_to_hex((digest), HEX), 0);                              \
    EXPECT_EQ_INT(hex_to_sha1(HEX, BACK), 0);                                  \
    EXPECT_EQ_INT(memcmp((digest), BACK, SHA1_DIGLEN), 0);                     \
  } while (0)

UTEST_CASE(sha1)
{
  unsigned char a[SHA1_DIGLEN];
  unsigned char b[SHA1_DIGLEN];
  unsigned char c[SHA1_DIGLEN];
  const unsigned char binary[] = {0x00, 0x01, 0xff, 0x00, 0x7f};

  EXPECT_EQ_INT(sha1("hello", 5, a), 0);
  EXPECT_EQ_INT(sha1("world", 5, b), 0);
  EXPECT_NE_INT(memcmp(a, b, SHA1_DIGLEN), 0);
  EXPECT_TRUE(digest_nonzero(a));

  EXPECT_EQ_INT(sha1("", 0, a), 0);
  EXPECT_TRUE(digest_nonzero(a));

  enum { LONG_LEN = 65536 };
  char *longbuf = malloc(LONG_LEN);

  EXPECT_NOTNULL(longbuf);
  memset(longbuf, 'x', LONG_LEN);
  EXPECT_EQ_INT(sha1(longbuf, LONG_LEN, a), 0);
  EXPECT_TRUE(digest_nonzero(a));
  longbuf[LONG_LEN - 1] = 'y';
  EXPECT_EQ_INT(sha1(longbuf, LONG_LEN, b), 0);
  EXPECT_NE_INT(memcmp(a, b, SHA1_DIGLEN), 0);
  free(longbuf);

  enum { LARGE_LEN = 1 << 20 };
  unsigned char *largebuf = malloc(LARGE_LEN);

  EXPECT_NOTNULL(largebuf);
  for (size_t i = 0; i < LARGE_LEN; i++)
    largebuf[i] = (unsigned char)(i & 0xff);
  EXPECT_EQ_INT(sha1(largebuf, LARGE_LEN, c), 0);
  EXPECT_TRUE(digest_nonzero(c));
  free(largebuf);

  EXPECT_EQ_INT(sha1("a", 1, a), 0);
  EXPECT_EQ_INT(sha1("b", 1, b), 0);
  EXPECT_NE_INT(memcmp(a, b, SHA1_DIGLEN), 0);

  EXPECT_EQ_INT(sha1("a\0b", 3, a), 0);
  EXPECT_EQ_INT(sha1("a", 1, b), 0);
  EXPECT_NE_INT(memcmp(a, b, SHA1_DIGLEN), 0);

  EXPECT_EQ_INT(sha1(binary, sizeof(binary), c), 0);
  EXPECT_TRUE(digest_nonzero(c));

  EXPECT_EQ_INT(sha1(binary, 0, a), 0);
  EXPECT_EQ_INT(sha1("", 0, b), 0);
  EXPECT_EQ_INT(memcmp(a, b, SHA1_DIGLEN), 0);
}

UTEST_CASE(sha1_to_hex)
{
  unsigned char digest[SHA1_DIGLEN];
  unsigned char hex[SHA1_HEXLEN];
  unsigned char hex2[SHA1_HEXLEN];
  const char *inputs[] = {"", "a", "hello", "The quick brown fox"};
  const size_t lens[] = {0, 1, 5, 19};

  EXPECT_EQ_INT(sha1("hello", 5, digest), 0);
  EXPECT_HEX_ROUNDTRIP(digest);
  EXPECT_EQ_INT(sha1_to_hex(digest, hex), 0);
  EXPECT_EQ_INT(sha1_to_hex(digest, hex2), 0);
  EXPECT_EQ_STR((char *)hex, (char *)hex2);
  EXPECT_EQ_INT(hex[SHA1_HEXLEN - 1], '\0');

  for (size_t i = 0; i < sizeof(inputs) / sizeof(*inputs); i++) {
    EXPECT_EQ_INT(sha1(inputs[i], lens[i], digest), 0);
    EXPECT_HEX_ROUNDTRIP(digest);
  }

  memset(digest, 0, SHA1_DIGLEN);
  EXPECT_HEX_ROUNDTRIP(digest);

  memset(digest, 0xff, SHA1_DIGLEN);
  EXPECT_HEX_ROUNDTRIP(digest);
}

UTEST_CASE(hex_to_sha1)
{
  unsigned char digest[SHA1_DIGLEN];
  unsigned char hex[SHA1_HEXLEN];
  unsigned char back[SHA1_DIGLEN];

  EXPECT_EQ_INT(sha1("payload", 7, digest), 0);
  EXPECT_EQ_INT(sha1_to_hex(digest, hex), 0);
  EXPECT_EQ_INT(hex_to_sha1(hex, back), 0);
  EXPECT_EQ_INT(memcmp(digest, back, SHA1_DIGLEN), 0);

  EXPECT_EQ_INT(hex_to_sha1(hex, back), 0);
  EXPECT_EQ_INT(memcmp(digest, back, SHA1_DIGLEN), 0);

  hex[0] = 'g';
  EXPECT_EQ_INT(hex_to_sha1(hex, back), -1);

  EXPECT_EQ_INT(sha1_to_hex(digest, hex), 0);
  hex[SHA1_DIGLEN * 2 - 1] = '\0';
  EXPECT_EQ_INT(hex_to_sha1(hex, back), -1);
}

UTEST_CASE(sha1_copy)
{
  unsigned char digest[SHA1_DIGLEN];
  unsigned char buf[SHA1_DIGLEN];
  unsigned char other[SHA1_DIGLEN];

  EXPECT_EQ_INT(sha1("hello", 5, digest), 0);
  EXPECT_EQ_PTR(sha1_copy(digest, buf), buf);
  EXPECT_EQ_INT(memcmp(digest, buf, SHA1_DIGLEN), 0);

  buf[0] ^= 0xff;
  EXPECT_NE_INT(memcmp(digest, buf, SHA1_DIGLEN), 0);
  EXPECT_EQ_PTR(sha1_copy(digest, buf), buf);
  EXPECT_EQ_INT(memcmp(digest, buf, SHA1_DIGLEN), 0);

  memset(other, 0, SHA1_DIGLEN);
  EXPECT_EQ_PTR(sha1_copy(other, buf), buf);
  EXPECT_EQ_INT(memcmp(other, buf, SHA1_DIGLEN), 0);

  errno = 0;
  EXPECT_NULL(sha1_copy(NULL, buf));
  EXPECT_EQ_INT(errno, EINVAL);

  errno = 0;
  EXPECT_NULL(sha1_copy(digest, NULL));
  EXPECT_EQ_INT(errno, EINVAL);
}

UTEST_CASE(sha1dup)
{
  unsigned char digest[SHA1_DIGLEN];
  unsigned char *dup;

  EXPECT_EQ_INT(sha1("hello", 5, digest), 0);
  dup = sha1dup(digest);
  EXPECT_NOTNULL(dup);
  EXPECT_NE_PTR(dup, digest);
  EXPECT_EQ_INT(memcmp(digest, dup, SHA1_DIGLEN), 0);

  dup[0] ^= 0xff;
  EXPECT_NE_INT(memcmp(digest, dup, SHA1_DIGLEN), 0);
  free(dup);

  dup = sha1dup(digest);
  EXPECT_NOTNULL(dup);
  EXPECT_EQ_INT(memcmp(digest, dup, SHA1_DIGLEN), 0);
  free(dup);

  errno = 0;
  EXPECT_NULL(sha1dup(NULL));
  EXPECT_EQ_INT(errno, EINVAL);
}
