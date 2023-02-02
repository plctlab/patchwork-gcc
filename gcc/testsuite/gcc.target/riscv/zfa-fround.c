/* { dg-do compile } */
/* { dg-skip-if "" { *-*-* } { "*" } { "-march=rv64* -O2" } } */
/* { dg-options "-march=rv64g_zfa -mabi=lp64d" } */

#include <math.h>

extern float a;
extern double b;

void foo (float *x, double *y)
{
  {
    *x = roundf (a);
    *y = round (b);
  }
  {
    *x = rintf (a);
    *y = rint (b);
  }
}

/* { dg-final { scan-assembler-times "fround.s" 1 } } */
/* { dg-final { scan-assembler-times "fround.d" 1 } } */
/* { dg-final { scan-assembler-times "froundnx.s" 1 } } */
/* { dg-final { scan-assembler-times "froundnx.d" 1 } } */
