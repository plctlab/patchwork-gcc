/* { dg-options "-O -fopenmp" } */

#include <stdint.h>
#include <arm_sve.h>

extern long N;
extern double *a, *b, *c;

void tuned_STREAM_Triad(double scalar)
{
  const uint64_t el = svcntd();
  const svfloat64_t vscalar = svdup_n_f64(scalar);
  const int Nadj = N/el;
  int j;

#pragma omp parallel for
  for (j = 0; j < Nadj; j ++) {
	  svfloat64_t va, vb, vc;
	  vb = svld1_vnum_f64(svptrue_b64(), b, j);
	  vc = svld1_vnum_f64(svptrue_b64(), c, j);
	  va = svmla_f64_z(svptrue_b64(), vb , vscalar, vc );
	  svst1_vnum_f64(svptrue_b64(), a, j+0, va);
  }

  for (j = Nadj*el ; j < N ; j++)
	  a[j] = b[j] + scalar * c[j];
}
