/* { dg-do compile } */
/* { dg-options "-march=rv64gc_xtheadbb -mabi=lp64" } */
/* { dg-skip-if "" { *-*-* } { "-O0" "-Os" "-Og" } } */

int
foo (int a, int b)
{
  return ((a & (1 << 25)) ? 5 : 4);
}

/* { dg-final { scan-assembler "extu\t" } } */
/* { dg-final { scan-assembler-not "andi" } } */
