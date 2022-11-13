/* { dg-do compile } */
/* { dg-options "-march=rv64gc_xtheadbs -mabi=lp64" } */
/* { dg-skip-if "" { *-*-* } { "-O0" } } */

long
foo1 (long i)
{
  return 1L & (i >> 20);
}

/* { dg-final { scan-assembler-times "th.tst\t" 1 } } */
/* { dg-final { scan-assembler-not "andi" } } */
