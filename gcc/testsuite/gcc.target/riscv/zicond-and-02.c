/* { dg-do compile } */
/* { dg-options "-march=rv64gc_zicond -mabi=lp64 -mbranch-cost=4" } */
/* { dg-skip-if "" { *-*-* } { "-O0" "-Og" "-Os" "-Oz" } } */

int and2(int a, int b, long c)
{
  if (c)
    a &= b;

  return a;
}

/* { dg-final { scan-assembler-times "and\t" 1 } } */
/* { dg-final { scan-assembler-times "czero.nez" 1 } } */
/* { dg-final { scan-assembler-times "or\t" 1 } } */
