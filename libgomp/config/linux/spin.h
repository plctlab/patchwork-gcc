static inline int
do_spin_for_count (int *addr, int val, unsigned long long count)
{
  unsigned long long i;
  for (i = 0; i < count; i++)
    if (__builtin_expect (__atomic_load_n (addr, MEMMODEL_RELAXED) != val, 0))
      return 0;
    else
      cpu_relax ();
  return 1;
}

