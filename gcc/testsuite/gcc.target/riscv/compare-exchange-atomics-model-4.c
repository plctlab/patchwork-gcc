/* { dg-do compile } */
/* Verify that appropriate bits are placed per memory model.  */
/* { dg-final { scan-assembler "lr.w.aqrl" } } */
/* { dg-final { scan-assembler "sc.w.rl" } } */
/* { dg-final { scan-assembler-not "lr.w.rl" } } */
/* { dg-final { scan-assembler-not "sc.w.aq" } } */

void
foo (int bar, int baz, int qux)
{
  __atomic_compare_exchange_n(&bar, &baz, qux, 1, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
}
