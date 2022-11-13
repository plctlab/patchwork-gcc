/* { dg-do compile } */
/* { dg-options "-O2 -march=rv32gc_xtheadmac -mabi=ilp32" } */
/* { dg-skip-if "" { *-*-* } { "-O0" "-O1" "-Og" } } */

long f_mula(long a, long b, long c)
{
  return a + b * c;
}

long f_muls(long a, long b, long c)
{
  return a - b * c;
}

long f_mulah(int a, unsigned short b, unsigned short c)
{
  return a + (int)(short)b * (int)(short)c;
}

long f_mulsh(int a, unsigned short b, unsigned short c)
{
  return a - (int)(short)b * (int)(short)c;
}

/* { dg-final { scan-assembler-times "th.mula\t" 1 } } */
/* { dg-final { scan-assembler-times "th.muls\t" 1 } } */
/* { dg-final { scan-assembler-times "th.mulah\t" 1 } } */
/* { dg-final { scan-assembler-times "th.mulsh\t" 1 } } */
