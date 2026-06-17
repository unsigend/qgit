#include "compress.h"

UTEST_SUITE(compress)
{
  UTEST_RUNCASE(zlib_compress);
  UTEST_RUNCASE(zlib_decompress);
  UTEST_RUNCASE(zlib_compressf);
  UTEST_RUNCASE(zlib_decompressf);
}
