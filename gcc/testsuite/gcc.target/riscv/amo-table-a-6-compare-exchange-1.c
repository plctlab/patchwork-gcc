/* { dg-do compile } */
/* Verify that compare exchange mappings match Table A.6's recommended mapping.  */
/* { dg-final { scan-assembler-not "lr.w.aq" } } */
/* { dg-final { scan-assembler-not "lr.w.rl" } } */
/* { dg-final { scan-assembler-not "sc.w.aq" } } */
/* { dg-final { scan-assembler-not "sc.w.rl" } } */

void
foo (int bar, int baz, int qux)
{
  __atomic_compare_exchange_n(&bar, &baz, qux, 1, __ATOMIC_RELAXED, __ATOMIC_RELAXED);
}
