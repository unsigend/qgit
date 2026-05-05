#include <ctype.h>
#include <sha1.h>
#include <string.h>
#include <utest.h>

UTEST_CASE(sha1_to_hex)
{
  unsigned char raw[20];
  unsigned char hex[41];
  unsigned char hex2[41];
  unsigned char round[20];
  size_t i;
  static const unsigned char pat[20] = {
      0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99,
      0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x01, 0xfe, 0x7f, 0x80};

  memset(raw, 0, sizeof raw);
  EXPECT_EQ_PTR(sha1_to_hex(raw, hex), hex);
  EXPECT_EQ_UINT(strlen((const char *)hex), 40u);
  EXPECT_EQ_UCHAR(hex[40], '\0');
  for (i = 0; i < 40; i++)
    EXPECT_TRUE(isxdigit(hex[i]));
  EXPECT_EQ_PTR(hex_to_sha1(hex, round), round);
  EXPECT_EQ_INT(memcmp(raw, round, sizeof raw), 0);

  for (i = 0; i < sizeof raw; i++)
    raw[i] = (unsigned char)(13u * i ^ (unsigned char)(i * 7 + 3));
  EXPECT_EQ_PTR(sha1_to_hex(raw, hex), hex);
  EXPECT_EQ_UINT(strlen((const char *)hex), 40u);
  EXPECT_EQ_UCHAR(hex[40], '\0');
  hex_to_sha1(hex, round);
  EXPECT_EQ_INT(memcmp(raw, round, sizeof raw), 0);

  memcpy(raw, pat, sizeof pat);
  EXPECT_EQ_PTR(sha1_to_hex(raw, hex), hex);
  EXPECT_EQ_PTR(sha1_to_hex(raw, hex2), hex2);
  EXPECT_EQ_INT(strcmp((const char *)hex, (const char *)hex2), 0);
  hex_to_sha1(hex, round);
  EXPECT_EQ_INT(memcmp(raw, round, sizeof raw), 0);

  EXPECT_EQ_PTR(sha1_hash((const unsigned char *)"opaque-bytes", 12, raw), raw);
  EXPECT_EQ_PTR(sha1_to_hex(raw, hex), hex);
  EXPECT_EQ_UINT(strlen((const char *)hex), 40u);
  hex_to_sha1(hex, round);
  EXPECT_EQ_INT(memcmp(raw, round, sizeof raw), 0);

  memset(raw, 0x3d, sizeof raw);
  sha1_to_hex(raw, hex);
  sha1_to_hex(raw, hex2);
  EXPECT_EQ_INT(memcmp(hex, hex2, sizeof hex), 0);
  hex_to_sha1(hex, round);
  EXPECT_EQ_INT(memcmp(raw, round, sizeof raw), 0);
}
