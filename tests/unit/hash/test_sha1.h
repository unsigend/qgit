#include <ctype.h>
#include <sha1.h>
#include <string.h>
#include <utest.h>

static int hex_val(unsigned char c)
{
  if (c >= '0' && c <= '9')
    return c - '0';
  if (c >= 'a' && c <= 'f')
    return c - 'a' + 10;
  return -1;
}

UTEST_CASE(sha1_hash)
{
  unsigned char out1[20];
  unsigned char out2[20];
  static const unsigned char a[] = "a";
  static const unsigned char b[] = "b";

  EXPECT_EQ_PTR(sha1_hash(a, 1, out1), out1);
  sha1_hash(a, 1, out2);
  EXPECT_EQ_INT(memcmp(out1, out2, 20), 0);

  sha1_hash(b, 1, out2);
  EXPECT_NE_INT(memcmp(out1, out2, 20), 0);
}

UTEST_CASE(sha1_hex)
{
  unsigned char raw[20];
  unsigned char hex1[41];
  unsigned char hex2[41];
  size_t i;

  memset(raw, 0, sizeof raw);
  EXPECT_EQ_PTR(sha1_hex(raw, hex1), hex1);
  EXPECT_EQ_UCHAR(hex1[40], '\0');
  EXPECT_EQ_UINT(strlen((const char *)hex1), 40u);
  for (i = 0; i < 40; i++)
    EXPECT_TRUE(isxdigit(hex1[i]));

  for (i = 0; i < 20; i++) {
    int hi = hex_val(hex1[i * 2]);
    int lo = hex_val(hex1[i * 2 + 1]);
    EXPECT_GE_INT(hi, 0);
    EXPECT_GE_INT(lo, 0);
    EXPECT_EQ_UCHAR((unsigned char)((hi << 4) | lo), raw[i]);
  }

  sha1_hex(raw, hex2);
  EXPECT_EQ_INT(strcmp((const char *)hex1, (const char *)hex2), 0);
}
