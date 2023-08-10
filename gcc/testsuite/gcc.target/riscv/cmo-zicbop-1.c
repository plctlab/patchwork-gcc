/* { dg-do compile target { { rv64-*-*}}} */
/* { dg-options "-march=rv64gc_zicbop -mabi=lp64" } */

void foo (char *p)
{
  __builtin_prefetch (p, 0, 0);
  __builtin_prefetch (p, 0, 1);
  __builtin_prefetch (p, 0, 2);
  __builtin_prefetch (p, 0, 3);
  __builtin_prefetch (p, 1, 0);
  __builtin_prefetch (p, 1, 1);
  __builtin_prefetch (p, 1, 2);
  __builtin_prefetch (p, 1, 3);
}

void foo1 ()
{
  __builtin_riscv_zicbop_prefetch_i(0);
  __builtin_riscv_zicbop_prefetch_i(&foo);
  __builtin_riscv_zicbop_prefetch_i((void*)0x111);
}

/* { dg-final { scan-assembler-times "prefetch.i" 3 } } */
/* { dg-final { scan-assembler-times "prefetch.r" 4 } } */
/* { dg-final { scan-assembler-times "prefetch.w" 4 } } */
