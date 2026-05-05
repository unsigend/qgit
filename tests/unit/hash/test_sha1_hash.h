#include <sha1.h>
#include <string.h>
#include <utest.h>

UTEST_CASE(sha1_hash)
{
  unsigned char buf1[20];
  unsigned char buf2[20];
  unsigned char blk[65];
  static const unsigned char mid[] =
      "repeat-this-payload-for-length-and-determinism";
  size_t i;

  EXPECT_EQ_PTR(sha1_hash((const unsigned char *)"", 0, buf1), buf1);
  EXPECT_EQ_PTR(sha1_hash((const unsigned char *)"", 0, buf2), buf2);
  EXPECT_EQ_INT(memcmp(buf1, buf2, 20), 0);

  EXPECT_EQ_PTR(sha1_hash((const unsigned char *)"qgit", 4, buf1), buf1);
  EXPECT_EQ_PTR(sha1_hash((const unsigned char *)"qgit", 4, buf2), buf2);
  EXPECT_EQ_INT(memcmp(buf1, buf2, 20), 0);

  EXPECT_EQ_PTR(sha1_hash((const unsigned char *)"a", 1, buf1), buf1);
  EXPECT_EQ_PTR(sha1_hash((const unsigned char *)"b", 1, buf2), buf2);
  EXPECT_NE_INT(memcmp(buf1, buf2, 20), 0);

  EXPECT_EQ_PTR(
      sha1_hash(mid, sizeof mid - 1, buf1), buf1);
  EXPECT_EQ_PTR(
      sha1_hash(mid, sizeof mid - 1, buf2), buf2);
  EXPECT_EQ_INT(memcmp(buf1, buf2, 20), 0);

  EXPECT_EQ_PTR(sha1_hash(mid, 6, buf1), buf1);
  EXPECT_EQ_PTR(sha1_hash(mid, 7, buf2), buf2);
  EXPECT_NE_INT(memcmp(buf1, buf2, 20), 0);

  for (i = 0; i < sizeof blk; i++)
    blk[i] = (unsigned char)('A' + (i % 26));
  EXPECT_EQ_PTR(sha1_hash(blk, 0, buf1), buf1);
  EXPECT_EQ_PTR(sha1_hash(blk, 0, buf2), buf2);
  EXPECT_EQ_INT(memcmp(buf1, buf2, 20), 0);

  EXPECT_EQ_PTR(sha1_hash(blk, 1, buf1), buf1);
  EXPECT_EQ_PTR(sha1_hash(blk, 1, buf2), buf2);
  EXPECT_EQ_INT(memcmp(buf1, buf2, 20), 0);

  EXPECT_EQ_PTR(sha1_hash(blk, 55, buf1), buf1);
  EXPECT_EQ_PTR(sha1_hash(blk, 55, buf2), buf2);
  EXPECT_EQ_INT(memcmp(buf1, buf2, 20), 0);

  EXPECT_EQ_PTR(sha1_hash(blk, 64, buf1), buf1);
  EXPECT_EQ_PTR(sha1_hash(blk, 64, buf2), buf2);
  EXPECT_EQ_INT(memcmp(buf1, buf2, 20), 0);

  EXPECT_EQ_PTR(sha1_hash(blk, 65, buf1), buf1);
  EXPECT_EQ_PTR(sha1_hash(blk, 65, buf2), buf2);
  EXPECT_EQ_INT(memcmp(buf1, buf2, 20), 0);

  EXPECT_EQ_PTR(sha1_hash(blk, 64, buf1), buf1);
  EXPECT_EQ_PTR(sha1_hash(blk, 65, buf2), buf2);
  EXPECT_NE_INT(memcmp(buf1, buf2, 20), 0);
}
