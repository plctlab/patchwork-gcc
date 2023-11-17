// Test basic behavior of a C++ static local variable vs. OpenMP.

#include <omp.h>
#include <unistd.h>

#define DEBUG_PRINTF //__builtin_printf

static int state;

static int cSC, cSD, cf;

struct S
{
  S()
  {
    DEBUG_PRINTF("%s\n", __PRETTY_FUNCTION__);

    int c;
#pragma omp atomic capture
    c = ++cSC;
    if (c != 1)
      __builtin_abort();

    if (state++ != 1)
      __builtin_abort();

    DEBUG_PRINTF("%s/\n", __PRETTY_FUNCTION__);
  }

  ~S()
  {
    DEBUG_PRINTF("%s\n", __PRETTY_FUNCTION__);

    int c;
#pragma omp atomic capture
    c = ++cSD;
    if (c != 1)
      __builtin_abort();

    if (state++ != 3)
      __builtin_abort();

    DEBUG_PRINTF("%s/\n", __PRETTY_FUNCTION__);
    // Exit '0', now that we've verified all is OK.
    _exit(0);
  }
};

static void f()
{
  DEBUG_PRINTF("%s\n", __PRETTY_FUNCTION__);

#pragma omp atomic
  ++cf;

  // <https://en.cppreference.com/w/cpp/language/storage_duration#Static_local_variables>
  static S s;

  DEBUG_PRINTF("%s/\n", __PRETTY_FUNCTION__);
}

int main()
{
  DEBUG_PRINTF("%s\n", __PRETTY_FUNCTION__);

  if (state++ != 0)
    __builtin_abort();

  int nthreads;

#pragma omp parallel
  {
#pragma omp master
    {
      nthreads = omp_get_num_threads ();
    }

    f();
  }

  DEBUG_PRINTF("  cSC = %d\n", cSC);
  DEBUG_PRINTF("  cf = %d\n", cf);
  if (cSC != 1)
    __builtin_abort();
  if (cf != nthreads)
    __builtin_abort();

  if (state++ != 2)
    __builtin_abort();

  DEBUG_PRINTF("%s/\n", __PRETTY_FUNCTION__);

  // See '_exit(0);' elsewhere.
  return 1;
}
