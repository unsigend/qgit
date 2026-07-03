#include <errno.h>
#include <libqgit/db/oid.h>
#include <stdlib.h>
#include <string.h>
#include <utest.h>

static const char *OID_HELLO =
    "9783073af922726808fe593d499f5feace949386";
static const char *OID_EMPTY_TREE =
    "4b825dc642cb6eb9a060e54bf8d69288fbee4904";
static const char *OID_DEADBEEF =
    "deadbeefdeadbeefdeadbeefdeadbeefdeadbeef";

static int oid_raw_eq(const qgit_oid *oid, const unsigned char *raw)
{
  return memcmp(oid->id, raw, QGIT_OID_RAWSZ) == 0;
}

static void oid_zero(qgit_oid *oid)
{
  memset(oid, 0, sizeof(*oid));
}

UTEST_CASE(qgit_oid_fromstr)
{
  qgit_oid oid;
  qgit_oid back;
  char hex[QGIT_OID_HEXSZ];

  EXPECT_EQ_INT(qgit_oid_fromstr(&oid, OID_HELLO), 0);
  EXPECT_EQ_INT(qgit_oid_fromstr(&back, OID_HELLO), 0);
  EXPECT_EQ_INT(qgit_oid_cmp(&oid, &back), 0);

  EXPECT_EQ_INT(qgit_oid_fromstr(&oid, OID_EMPTY_TREE), 0);
  EXPECT_EQ_INT(qgit_oid_fromstr(&oid, OID_DEADBEEF), 0);

  memset(hex, 0, sizeof(hex));
  EXPECT_EQ_INT(qgit_oid_fmt(hex, &oid), 0);
  EXPECT_EQ_INT(strncmp(hex, OID_DEADBEEF, QGIT_OID_RAWSZ * 2), 0);

  {
    char upper[QGIT_OID_RAWSZ * 2 + 1];
    strcpy(upper, OID_HELLO);
    for (size_t i = 0; i < QGIT_OID_RAWSZ * 2; i++)
      if (upper[i] >= 'a' && upper[i] <= 'f')
        upper[i] = (char)(upper[i] - 'a' + 'A');
    EXPECT_EQ_INT(qgit_oid_fromstr(&oid, upper), 0);
    EXPECT_EQ_INT(qgit_oid_fromstr(&back, OID_HELLO), 0);
    EXPECT_EQ_INT(qgit_oid_cmp(&oid, &back), 0);
  }

  errno = 0;
  EXPECT_EQ_INT(qgit_oid_fromstr(&oid, "9783073af922726808fe593d499f5feace94938"), -1);
  EXPECT_NE_INT(errno, 0);

  errno = 0;
  EXPECT_EQ_INT(qgit_oid_fromstr(&oid, "9783073af922726808fe593d499f5feace949386g"), -1);
  EXPECT_NE_INT(errno, 0);

  errno = 0;
  EXPECT_EQ_INT(qgit_oid_fromstr(&oid, "zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz"), -1);
  EXPECT_NE_INT(errno, 0);

  errno = 0;
  EXPECT_EQ_INT(qgit_oid_fromstr(&oid, ""), -1);
  EXPECT_NE_INT(errno, 0);
}

UTEST_CASE(qgit_oid_fromstrn)
{
  qgit_oid oid;
  qgit_oid full;
  qgit_oid expected;

  EXPECT_EQ_INT(qgit_oid_fromstr(&full, OID_HELLO), 0);

  oid_zero(&oid);
  EXPECT_EQ_INT(qgit_oid_fromstrn(&oid, OID_HELLO, QGIT_OID_RAWSZ * 2), 0);
  EXPECT_EQ_INT(qgit_oid_cmp(&oid, &full), 0);

  oid_zero(&oid);
  EXPECT_EQ_INT(qgit_oid_fromstrn(&oid, OID_HELLO, 7), 0);
  EXPECT_EQ_INT(qgit_oid_fromstrn(&expected, OID_HELLO, 6), 0);
  EXPECT_EQ_INT(qgit_oid_cmp(&oid, &expected), 0);
  for (size_t i = 3; i < QGIT_OID_RAWSZ; i++)
    EXPECT_EQ_UCHAR(oid.id[i], 0);

  oid_zero(&oid);
  EXPECT_EQ_INT(qgit_oid_fromstrn(&oid, OID_HELLO, 8), 0);
  EXPECT_EQ_INT(qgit_oid_fromstrn(&expected, OID_HELLO, 8), 0);
  EXPECT_EQ_INT(qgit_oid_cmp(&oid, &expected), 0);
  for (size_t i = 4; i < QGIT_OID_RAWSZ; i++)
    EXPECT_EQ_UCHAR(oid.id[i], 0);

  oid_zero(&oid);
  EXPECT_EQ_INT(qgit_oid_fromstrn(&oid, OID_HELLO, QGIT_OID_MINPREFIXLEN), 0);
  EXPECT_EQ_INT(qgit_oid_ncmp(&oid, &full, QGIT_OID_MINPREFIXLEN), 0);
  EXPECT_NE_INT(qgit_oid_cmp(&oid, &full), 0);

  oid_zero(&oid);
  EXPECT_EQ_INT(qgit_oid_fromstrn(&oid, OID_HELLO, 0), 0);
  EXPECT_EQ_INT(qgit_oid_iszero(&oid), 1);

  errno = 0;
  oid_zero(&oid);
  EXPECT_EQ_INT(qgit_oid_fromstrn(&oid, "9783073af922726808fe593d499f5feace949386g", 41), -1);
  EXPECT_NE_INT(errno, 0);

  errno = 0;
  oid_zero(&oid);
  EXPECT_EQ_INT(qgit_oid_fromstrn(&oid, "gggg", 4), -1);
  EXPECT_NE_INT(errno, 0);
}

UTEST_CASE(qgit_oid_fromraw)
{
  qgit_oid oid;
  qgit_oid parsed;
  unsigned char raw[QGIT_OID_RAWSZ];
  char hex[QGIT_OID_HEXSZ];

  for (size_t i = 0; i < QGIT_OID_RAWSZ; i++)
    raw[i] = (unsigned char)(i * 13 + 7);

  qgit_oid_fromraw(&oid, raw);
  EXPECT_TRUE(oid_raw_eq(&oid, raw));

  EXPECT_EQ_INT(qgit_oid_fromstr(&parsed, OID_HELLO), 0);
  qgit_oid_fromraw(&oid, parsed.id);
  EXPECT_EQ_INT(qgit_oid_cmp(&oid, &parsed), 0);

  memset(raw, 0, QGIT_OID_RAWSZ);
  qgit_oid_fromraw(&oid, raw);
  EXPECT_EQ_INT(qgit_oid_iszero(&oid), 1);

  memset(raw, 0xff, QGIT_OID_RAWSZ);
  qgit_oid_fromraw(&oid, raw);
  memset(hex, 0, sizeof(hex));
  EXPECT_EQ_INT(qgit_oid_fmt(hex, &oid), 0);
  for (size_t i = 0; i < QGIT_OID_RAWSZ * 2; i++)
    EXPECT_EQ_CHAR(hex[i], 'f');
}

UTEST_CASE(qgit_oid_fmt)
{
  qgit_oid oid;
  char hex[QGIT_OID_HEXSZ];
  char backhex[QGIT_OID_HEXSZ];
  qgit_oid back;

  EXPECT_EQ_INT(qgit_oid_fromstr(&oid, OID_HELLO), 0);
  memset(hex, 'x', sizeof(hex));
  EXPECT_EQ_INT(qgit_oid_fmt(hex, &oid), 0);
  EXPECT_EQ_INT(strncmp(hex, OID_HELLO, QGIT_OID_RAWSZ * 2), 0);
  EXPECT_EQ_CHAR(hex[QGIT_OID_RAWSZ * 2], 'x');

  EXPECT_EQ_INT(qgit_oid_fromstr(&oid, OID_EMPTY_TREE), 0);
  memset(hex, 0, sizeof(hex));
  EXPECT_EQ_INT(qgit_oid_fmt(hex, &oid), 0);
  EXPECT_EQ_STR(hex, OID_EMPTY_TREE);

  oid_zero(&oid);
  EXPECT_EQ_INT(qgit_oid_fmt(hex, &oid), 0);
  for (size_t i = 0; i < QGIT_OID_RAWSZ * 2; i++)
    EXPECT_EQ_CHAR(hex[i], '0');

  for (size_t i = 0; i < QGIT_OID_RAWSZ; i++)
    oid.id[i] = (unsigned char)(0xa0 + i);
  EXPECT_EQ_INT(qgit_oid_fmt(hex, &oid), 0);
  EXPECT_EQ_INT(qgit_oid_fromstr(&back, hex), 0);
  EXPECT_EQ_INT(qgit_oid_cmp(&oid, &back), 0);

  EXPECT_EQ_INT(qgit_oid_fromstr(&oid, OID_DEADBEEF), 0);
  EXPECT_EQ_INT(qgit_oid_fmt(backhex, &oid), 0);
  EXPECT_EQ_INT(qgit_oid_fromstr(&back, backhex), 0);
  EXPECT_EQ_INT(qgit_oid_cmp(&oid, &back), 0);
}

UTEST_CASE(qgit_oid_fmtpath)
{
  qgit_oid oid;
  char path[QGIT_OID_HEXSZ + 1];
  char hex[QGIT_OID_HEXSZ];

  EXPECT_EQ_INT(qgit_oid_fromstr(&oid, OID_HELLO), 0);
  memset(path, 0, sizeof(path));
  EXPECT_EQ_INT(qgit_oid_fmtpath(path, &oid), 0);
  EXPECT_EQ_CHAR(path[2], '/');
  EXPECT_EQ_INT(strncmp(path, OID_HELLO, 2), 0);
  EXPECT_EQ_INT(strncmp(path + 3, OID_HELLO + 2, QGIT_OID_RAWSZ * 2 - 2), 0);

  EXPECT_EQ_INT(qgit_oid_fmt(hex, &oid), 0);
  EXPECT_EQ_CHAR(path[0], hex[0]);
  EXPECT_EQ_CHAR(path[1], hex[1]);
  EXPECT_EQ_CHAR(path[3], hex[2]);

  EXPECT_EQ_INT(qgit_oid_fromstr(&oid, OID_EMPTY_TREE), 0);
  EXPECT_EQ_INT(qgit_oid_fmtpath(path, &oid), 0);
  EXPECT_EQ_STR(path, "4b/825dc642cb6eb9a060e54bf8d69288fbee4904");

  EXPECT_EQ_INT(qgit_oid_fromstr(&oid, OID_DEADBEEF), 0);
  EXPECT_EQ_INT(qgit_oid_fmtpath(path, &oid), 0);
  EXPECT_EQ_STR(path, "de/adbeefdeadbeefdeadbeefdeadbeefdeadbeef");
}

UTEST_CASE(qgit_oid_strdup)
{
  qgit_oid oid;
  char *dup;
  char hex[QGIT_OID_HEXSZ];

  EXPECT_EQ_INT(qgit_oid_fromstr(&oid, OID_HELLO), 0);
  dup = qgit_oid_strdup(&oid);
  EXPECT_NOTNULL(dup);
  EXPECT_EQ_STR(dup, OID_HELLO);
  EXPECT_EQ_INT(qgit_oid_fromstr(&oid, OID_EMPTY_TREE), 0);
  free(dup);

  dup = qgit_oid_strdup(&oid);
  EXPECT_NOTNULL(dup);
  EXPECT_EQ_STR(dup, OID_EMPTY_TREE);
  free(dup);

  oid_zero(&oid);
  dup = qgit_oid_strdup(&oid);
  EXPECT_NOTNULL(dup);
  EXPECT_EQ_INT(strlen(dup), QGIT_OID_RAWSZ * 2);
  for (size_t i = 0; i < QGIT_OID_RAWSZ * 2; i++)
    EXPECT_EQ_CHAR(dup[i], '0');
  free(dup);

  EXPECT_EQ_INT(qgit_oid_fromstr(&oid, OID_DEADBEEF), 0);
  dup = qgit_oid_strdup(&oid);
  EXPECT_NOTNULL(dup);
  memset(hex, 0, sizeof(hex));
  EXPECT_EQ_INT(qgit_oid_fmt(hex, &oid), 0);
  EXPECT_EQ_STR(dup, hex);
  dup[0] = 'x';
  EXPECT_EQ_INT(qgit_oid_fmt(hex, &oid), 0);
  EXPECT_NE_CHAR(hex[0], 'x');
  free(dup);
}

UTEST_CASE(qgit_oid_copy)
{
  qgit_oid src;
  qgit_oid dest;

  EXPECT_EQ_INT(qgit_oid_fromstr(&src, OID_HELLO), 0);
  oid_zero(&dest);
  qgit_oid_copy(&dest, &src);
  EXPECT_EQ_INT(qgit_oid_cmp(&dest, &src), 0);

  dest.id[0] ^= 0xff;
  EXPECT_NE_INT(qgit_oid_cmp(&dest, &src), 0);
  qgit_oid_copy(&dest, &src);
  EXPECT_EQ_INT(qgit_oid_cmp(&dest, &src), 0);

  EXPECT_EQ_INT(qgit_oid_fromstr(&src, OID_DEADBEEF), 0);
  qgit_oid_copy(&dest, &src);
  EXPECT_EQ_INT(qgit_oid_cmp(&dest, &src), 0);
  src.id[QGIT_OID_RAWSZ - 1] = 0x00;
  EXPECT_NE_INT(qgit_oid_cmp(&dest, &src), 0);

  oid_zero(&src);
  qgit_oid_copy(&dest, &src);
  EXPECT_EQ_INT(qgit_oid_iszero(&dest), 1);
}

UTEST_CASE(qgit_oid_cmp)
{
  qgit_oid a;
  qgit_oid b;
  qgit_oid c;

  EXPECT_EQ_INT(qgit_oid_fromstr(&a, OID_HELLO), 0);
  EXPECT_EQ_INT(qgit_oid_fromstr(&b, OID_HELLO), 0);
  EXPECT_EQ_INT(qgit_oid_cmp(&a, &b), 0);
  EXPECT_EQ_INT(qgit_oid_cmp(&b, &a), 0);

  EXPECT_EQ_INT(qgit_oid_fromstr(&b, OID_EMPTY_TREE), 0);
  EXPECT_LT_INT(qgit_oid_cmp(&b, &a), 0);
  EXPECT_GT_INT(qgit_oid_cmp(&a, &b), 0);

  EXPECT_EQ_INT(qgit_oid_fromstr(&c, OID_DEADBEEF), 0);
  EXPECT_LT_INT(qgit_oid_cmp(&a, &c), 0);
  EXPECT_GT_INT(qgit_oid_cmp(&c, &a), 0);
  EXPECT_LT_INT(qgit_oid_cmp(&b, &c), 0);
  EXPECT_GT_INT(qgit_oid_cmp(&c, &b), 0);

  oid_zero(&a);
  EXPECT_EQ_INT(qgit_oid_fromstr(&b, OID_HELLO), 0);
  EXPECT_LT_INT(qgit_oid_cmp(&a, &b), 0);
  EXPECT_GT_INT(qgit_oid_cmp(&b, &a), 0);
  EXPECT_EQ_INT(qgit_oid_cmp(&a, &a), 0);
}

UTEST_CASE(qgit_oid_ncmp)
{
  qgit_oid a;
  qgit_oid b;
  qgit_oid prefix;

  EXPECT_EQ_INT(qgit_oid_fromstr(&a, OID_HELLO), 0);
  EXPECT_EQ_INT(qgit_oid_fromstr(&b, OID_HELLO), 0);
  EXPECT_EQ_INT(qgit_oid_ncmp(&a, &b, QGIT_OID_RAWSZ * 2), 0);
  EXPECT_EQ_INT(qgit_oid_ncmp(&a, &b, 0), 0);

  EXPECT_EQ_INT(qgit_oid_fromstrn(&prefix, OID_HELLO, 7), 0);
  EXPECT_EQ_INT(qgit_oid_ncmp(&prefix, &a, 7), 0);
  EXPECT_NE_INT(qgit_oid_ncmp(&prefix, &a, 8), 0);

  EXPECT_EQ_INT(qgit_oid_fromstr(&b, OID_EMPTY_TREE), 0);
  EXPECT_EQ_INT(qgit_oid_ncmp(&a, &b, 1), 0);
  EXPECT_NE_INT(qgit_oid_ncmp(&a, &b, 2), 0);

  EXPECT_EQ_INT(qgit_oid_fromstr(&a, OID_DEADBEEF), 0);
  EXPECT_EQ_INT(qgit_oid_fromstr(&b, "deadbeefdeadbeefdeadbeefdeadbeefdeadbeed"), 0);
  EXPECT_EQ_INT(qgit_oid_ncmp(&a, &b, QGIT_OID_RAWSZ * 2 - 1), 0);
  EXPECT_NE_INT(qgit_oid_ncmp(&a, &b, QGIT_OID_RAWSZ * 2), 0);

  EXPECT_EQ_INT(qgit_oid_ncmp(&a, &b, QGIT_OID_RAWSZ * 2 + 8), 0);
}

UTEST_CASE(qgit_oid_iszero)
{
  qgit_oid oid;

  oid_zero(&oid);
  EXPECT_EQ_INT(qgit_oid_iszero(&oid), 1);

  EXPECT_EQ_INT(qgit_oid_fromstr(&oid, OID_HELLO), 0);
  EXPECT_EQ_INT(qgit_oid_iszero(&oid), 0);

  oid_zero(&oid);
  EXPECT_EQ_INT(qgit_oid_fromstrn(&oid, OID_HELLO, QGIT_OID_MINPREFIXLEN), 0);
  EXPECT_EQ_INT(qgit_oid_iszero(&oid), 0);

  oid_zero(&oid);
  oid.id[0] = 0x01;
  EXPECT_EQ_INT(qgit_oid_iszero(&oid), 0);

  oid_zero(&oid);
  oid.id[QGIT_OID_RAWSZ - 1] = 0x01;
  EXPECT_EQ_INT(qgit_oid_iszero(&oid), 0);

  {
    unsigned char raw[QGIT_OID_RAWSZ];
    memset(raw, 0, QGIT_OID_RAWSZ);
    qgit_oid_fromraw(&oid, raw);
    EXPECT_EQ_INT(qgit_oid_iszero(&oid), 1);
  }
}
