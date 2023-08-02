#ifdef __x86_64__
static inline void
gomp_thread_delay(unsigned int count)
{
  unsigned long long i;
  for (i = 0; i < count * gomp_thread_delay_count; i++)
    __builtin_ia32_pause ();
}

#define RUNLOCALFN(a, b, c)  \
  do \
    { \
      gomp_thread_delay(c); \
      a (b); \
    } \
  while (0)
#else
# include "../../../../include/localfn.h"
#endif
