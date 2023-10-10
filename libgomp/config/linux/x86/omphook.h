#ifdef __x86_64__
#include "cpuid.h"

/* only for x86 hybrid platform */
#define RUNOMPHOOK()  \
  do \
    { \
      unsigned int eax, ebx, ecx, edx; \
      if ((getenv ("GOMP_SPINCOUNT") == NULL) && (wait_policy < 0) \
	  && __get_cpuid_count (7, 0, &eax, &ebx, &ecx, &edx) \
	  && ((edx >> 15) & 1)) \
	gomp_spin_count_var = 1LL; \
      if (gomp_throttled_spin_count_var > gomp_spin_count_var) \
	gomp_throttled_spin_count_var = gomp_spin_count_var; \
    } \
  while (0)
#else
# include "../../../../include/omphook.h"
#endif
