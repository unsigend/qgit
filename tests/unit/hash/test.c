#include "test_sha1.h"

UTEST_SUITE(hash)
{
  UTEST_RUNCASE(sha1_hash);
  UTEST_RUNCASE(sha1_hex);
}