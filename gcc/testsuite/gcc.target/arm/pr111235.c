/* { dg-do compile } */
/* { dg-options "-std=c11 -O" } */
/* { dg-require-effective-target arm_arch_v7a_ok } */
/* { dg-add-options arm_arch_v7a } */

int t0 (int *p, int x)
{
  if (x > 100)
    x = atomic_load_explicit (p, memory_order_relaxed);
  return x + 1;
}

long long t1 (long long *p, int x)
{
  if (x > 100)
    x = atomic_load_explicit (p, memory_order_relaxed);
  return x + 1;
}

void t2 (int *p, int x)
{
  if (x > 100)
    atomic_store_explicit (p, x, memory_order_relaxed);
}

void t3 (long long *p, long long x)
{
  if (x > 100)
    atomic_store_explicit (p, x, memory_order_relaxed);
}


/* { dg-final { scan-assembler-times "ldrexd\tr\[0-9\]+, r\[0-9\]+, \\\[r\[0-9\]+\\\]" 1 } } */
/* { dg-final { scan-assembler-times "dmb\tish" 1 } } */

