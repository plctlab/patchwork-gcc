/* { dg-do compile } */
/* { dg-options "-march=rv64gc_xtheadba -mabi=lp64" } */
/* { dg-skip-if "" { *-*-* } { "-O0" } } */

long test_1(long a, long b)
{
  return a + (b << 1);
}
long test_2(long a, long b)
{
  return a + (b << 2);
}
long test_3(long a, long b)
{
  return a + (b << 3);
}

/* { dg-final { scan-assembler-times "th.addsl\[ \t\]*a\[0-9\]+,a\[0-9\]+,a\[0-9\]+,1" 1 } } */
/* { dg-final { scan-assembler-times "th.addsl\[ \t\]*a\[0-9\]+,a\[0-9\]+,a\[0-9\]+,2" 1 } } */
/* { dg-final { scan-assembler-times "th.addsl\[ \t\]*a\[0-9\]+,a\[0-9\]+,a\[0-9\]+,3" 1 } } */
