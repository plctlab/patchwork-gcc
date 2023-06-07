/* { dg-do compile } */
/* { dg-options "-mtune=veyron-v1 -mabi=lp64" } */
/* { dg-skip-if "" { *-*-* } { "-O0" "-O1" "-Og" "-Oz" "-Os" } } */

void
foo(int a, int b, int *c, int *d)
{
   *c = a / b;
   *d = a % b;
}

/* { dg-final { scan-assembler-not "\trem" } } */
/* { dg-final { scan-assembler-times "\tdiv" 1 } } */
/* { dg-final { scan-assembler-times "\tmul" 1 } } */
/* { dg-final { scan-assembler-times "\tsub" 1 } } */
