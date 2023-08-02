#include "../mutex.c"

#ifdef __x86_64__
static inline int
do_spin_for_count_generic (int *addr, int val, unsigned long long count)
{
  unsigned long long i;
  for (i = 0; i < count; i++)
    if (__builtin_expect (__atomic_load_n (addr, MEMMODEL_RELAXED) != val,
			  0))
      return 0;
    else
      cpu_relax ();
  return 1;
}

#ifndef __WAITPKG__
#pragma GCC push_options
#pragma GCC target("waitpkg")
#define __DISABLE_WAITPKG__
#endif /* __WAITPKG__ */

static inline unsigned long long __rdtsc(void)
{
  unsigned long long var;
  unsigned int hi, lo;

  __asm volatile ("rdtsc" : "=a" (lo), "=d" (hi));

  var = ((unsigned long long)hi << 32) | lo;
  return var;
}

#define PAUSE_TP 200
static inline int
do_spin_for_backoff_tpause (int *addr, int val, unsigned long long count)
{
  unsigned int ctrl = 1;
  unsigned long long wait_time = 1;
  unsigned long long mask = 1ULL << __builtin_ia32_bsrdi(count * PAUSE_TP);
  do
    {
      __builtin_ia32_tpause (ctrl, wait_time + __rdtsc());
      wait_time = (wait_time << 1) | 1;
      if (__builtin_expect (__atomic_load_n (addr, MEMMODEL_RELAXED) != val,
			    0))
	return 0;
    }
  while ((wait_time & mask) == 0);
  return 1;
}

#ifdef __DISABLE_WAITPKG__
#undef __DISABLE_WAITPKG__
#pragma GCC pop_options
#endif /* __DISABLE_WAITPKG__ */

int do_spin_for_count (int *addr, int val, unsigned long long count)
{
  if(__builtin_cpu_supports ("waitpkg"))
    return do_spin_for_backoff_tpause(addr, val, count);
  else
    return do_spin_for_count_generic(addr, val, count);
}

#endif
