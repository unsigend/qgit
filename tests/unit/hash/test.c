#include "test_hex_to_sha1.h"
#include "test_sha1_hash.h"
#include "test_sha1_to_hex.h"

UTEST_SUITE(hash)
{
  UTEST_RUNCASE(sha1_hash);
  UTEST_RUNCASE(sha1_to_hex);
  UTEST_RUNCASE(hex_to_sha1);
}
