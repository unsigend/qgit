#include "oid.h"

UTEST_SUITE(oid)
{
  UTEST_RUNCASE(qgit_oid_fromstr);
  UTEST_RUNCASE(qgit_oid_fromstrn);
  UTEST_RUNCASE(qgit_oid_fromraw);
  UTEST_RUNCASE(qgit_oid_fmt);
  UTEST_RUNCASE(qgit_oid_fmtpath);
  UTEST_RUNCASE(qgit_oid_strdup);
  UTEST_RUNCASE(qgit_oid_copy);
  UTEST_RUNCASE(qgit_oid_cmp);
  UTEST_RUNCASE(qgit_oid_ncmp);
  UTEST_RUNCASE(qgit_oid_iszero);
}
