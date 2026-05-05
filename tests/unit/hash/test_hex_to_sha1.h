#include <sha1.h>
#include <string.h>
#include <utest.h>

UTEST_CASE(hex_to_sha1)
{
  unsigned char raw[20];
  unsigned char back[20];
  unsigned char hex[41];
  unsigned char blk[80];
  static const unsigned char p0[] = "";
  static const unsigned char p1[] = "unit";
  static const unsigned char p2[] = "hex-to-raw-round-trip-probe";
  size_t i;

  EXPECT_EQ_PTR(sha1_hash(p0, 0, raw), raw);
  EXPECT_EQ_PTR(sha1_to_hex(raw, hex), hex);
  EXPECT_EQ_PTR(hex_to_sha1(hex, back), back);
  EXPECT_EQ_INT(memcmp(raw, back, sizeof raw), 0);

  EXPECT_EQ_PTR(sha1_hash(p1, sizeof p1 - 1, raw), raw);
  sha1_to_hex(raw, hex);
  EXPECT_EQ_PTR(hex_to_sha1(hex, back), back);
  EXPECT_EQ_INT(memcmp(raw, back, sizeof raw), 0);

  EXPECT_EQ_PTR(sha1_hash(p2, sizeof p2 - 1, raw), raw);
  sha1_to_hex(raw, hex);
  hex_to_sha1(hex, back);
  EXPECT_EQ_INT(memcmp(raw, back, sizeof raw), 0);

  for (i = 0; i < sizeof blk; i++)
    blk[i] = (unsigned char)(i * 17u + 3u);
  EXPECT_EQ_PTR(sha1_hash(blk, sizeof blk, raw), raw);
  sha1_to_hex(raw, hex);
  hex_to_sha1(hex, back);
  EXPECT_EQ_INT(memcmp(raw, back, sizeof raw), 0);

  sha1_to_hex(raw, hex);
  EXPECT_EQ_PTR(hex_to_sha1(hex, back), back);
  EXPECT_EQ_PTR(hex_to_sha1(hex, raw), raw);
  EXPECT_EQ_INT(memcmp(raw, back, sizeof raw), 0);
}
