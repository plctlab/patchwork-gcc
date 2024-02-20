/* { dg-do run { target { lp64 } } } */
/* { dg-require-effective-target powerpc_vsx_ok } */
/* { dg-options "-O2 -mdejagnu-cpu=power7" } */

#define DEBUG 0

#if DEBUG
#include <stdio.h>
#include <stdlib.h>
#endif

void abort (void);

int main ()
{
  int i;
  vector double vd_arg1, vd_result, vd_expected_result;
  vector float vf_arg1, vf_result, vf_expected_result;

  /* VSX Vector Negate Single-Precision.  */

  vf_arg1 = (vector float) {-1.0, 12345.98, -2.1234, 238.9};
  vf_result = __builtin_vsx_xvnegsp (vf_arg1);
  vf_expected_result = (vector float) {1.0, -12345.98, 2.1234, -238.9};

  for (i = 0; i < 4; i++)
    if (vf_result[i] != vf_expected_result[i])
#if DEBUG
      printf("ERROR, __builtin_vsx_xvnegsp: vf_result[%d] = %f, vf_expected_result[%d] = %f\n",
	     i, vf_result[i], i, vf_expected_result[i]);
#else
      abort();
#endif

  /* VSX Vector Negate Double-Precision.  */

  vd_arg1 = (vector double) {12345.98, -2.1234};
  vd_result = __builtin_vsx_xvnegdp (vd_arg1);
  vd_expected_result = (vector double) {-12345.98, 2.1234};

  for (i = 0; i < 2; i++)
    if (vd_result[i] != vd_expected_result[i])
#if DEBUG
      printf("ERROR, __builtin_vsx_xvnegdp: vd_result[%d] = %f, vd_expected_result[%d] = %f\n",
	     i, vd_result[i], i, vd_expected_result[i]);
#else
      abort();
#endif

  return 0;
}
