/* { dg-do compile } */
/* { dg-options "-msse2 -O2" } */
/* { dg-final { scan-assembler-times "pslld" 1 } } */
/* { dg-final { scan-assembler-times "psrld" 1 } } */

float
extendsfbf (__bf16 a)
{
  return a;
}

__bf16
truncsfbf (float a)
{
  return a;
}
