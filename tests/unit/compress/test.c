#include "test_compress.h"

UTEST_SUITE(compress)
{
  UTEST_RUNCASE(zlib_round_trip_empty);
  UTEST_RUNCASE(zlib_round_trip_short);
  UTEST_RUNCASE(zlib_round_trip_zeros_large);
  UTEST_RUNCASE(zlib_round_trip_pattern);
  UTEST_RUNCASE(zlib_round_trip_two_payloads);
  UTEST_RUNCASE(zlib_decompress_invalid);
  UTEST_RUNCASE(zlib_round_trip_single_byte);
  UTEST_RUNCASE(zlib_round_trip_byte_spectrum);
  UTEST_RUNCASE(zlib_round_trip_utf8);
  UTEST_RUNCASE(zlib_decompress_empty_source);
  UTEST_RUNCASE(zlib_decompress_truncated);
  UTEST_RUNCASE(zlib_compress_deterministic);
}
