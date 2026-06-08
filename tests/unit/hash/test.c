#include "hash.h"

UTEST_SUITE(hash)
{
  UTEST_RUNCASE(sha1);
  UTEST_RUNCASE(sha1_to_hex);
  UTEST_RUNCASE(hex_to_sha1);
}
