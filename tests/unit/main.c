/* Test Runner for collection library
   Main entry point for the testing framework.

   Usage: ./test -j[nthreads] [suite] */

#include <stdio.h>
#include <stdlib.h>
#include <utest.h>

#define NTHREADS 5
#define NTHREADMAX 64

extern UTEST_SUITE(hash);

int main(int argc, char *argv[])
{
  int nthreads = NTHREADS;
  const char *suite = "all";

  UTEST_INIT(UTF_STOPONASS | UTF_SHOWSUITE | UTF_STOPONCASE);

  /* add test suites */
  UTEST_ADDSUITE(hash);

  for (int i = 1; i < argc; i++) {
    if (strncmp(argv[i], "-j", 2) == 0) {
      nthreads = atoi(argv[i] + 2);
      if (nthreads <= 0 || nthreads > NTHREADMAX) {
        fprintf(stderr, "invalid number of threads: %s\n", argv[i] + 2);
        return EXIT_FAILURE;
      }
    } else
      suite = argv[i];
  }

  if (!strcmp(suite, "all")) {
    fprintf(stdout, "Util: running all test suites with %d threads\n",
            nthreads);
    UTEST_RUNSUITES_THREAD(nthreads);
  } else {
    UTEST_ADDFLAG(UTF_SHOWCASE);
    if (UTEST_RUNSUITE(suite) == -1) {
      UTEST_SHOWSUITES();
      return EXIT_FAILURE;
    }
  }

  UTEST_FINI();
  return EXIT_SUCCESS;
}