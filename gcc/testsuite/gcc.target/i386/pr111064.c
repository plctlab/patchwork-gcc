/* { dg-do compile } */
/* { dg-options "-Ofast -march=icelake-server -mno-gather" } */
/* { dg-final { scan-assembler-times {(?n)vfmadd[123]*pd.*ymm} 2 { target { ! ia32 } } } }  */

double
foo (double* a, double* b, unsigned int* c, int n)
{
  double sum = 0;
  for (int i = 0; i != n; i++)
    sum += a[i] * b[c[i]];
  return sum;
}
