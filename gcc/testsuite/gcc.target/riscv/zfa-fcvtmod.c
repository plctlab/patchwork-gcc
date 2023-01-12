/* { dg-do compile } */
/* { dg-skip-if "" { *-*-* } { "*" } { "-march=rv64* -O2" } } */
/* { dg-options "-march=rv64g_zfa -mabi=lp64d" } */

extern double a;

void foo(int *x, double a)
{
  *x = a;
}

/* { dg-final { scan-assembler-times "fcvtmod.w.d" 1 } } */