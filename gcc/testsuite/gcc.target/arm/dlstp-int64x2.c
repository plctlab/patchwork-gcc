/* { dg-do run { target { arm*-*-* } } } */
/* { dg-require-effective-target arm_v8_1m_mve_ok } */
/* { dg-skip-if "avoid conflicting multilib options" { *-*-* } { "-marm" "-mcpu=*" } } */
/* { dg-options "-march=armv8.1-m.main+fp.dp+mve.fp -mfloat-abi=hard -mfpu=auto -O3 --save-temps" } */

#include <arm_mve.h>
#include <stdio.h>
#include <stdlib.h>
#include "lob.h"

void  __attribute__ ((noinline)) test (int64_t *a, int64_t *c, int n)
{
  while (n > 0)
    {
      mve_pred16_t p = vctp64q (n);
      int64x2_t va = vldrdq_gather_offset_z_s64 (a, vcreateq_u64 (8, 0), p);
      vstrdq_scatter_offset_p_s64 (c, vcreateq_u64 (8, 0), va, p);
      c+=2;
      a+=2;
      n-=2;
    }
}

int main ()
{
  int i;
  int64_t temp1[N];
  int64_t temp3[N];
  reset_data64  (temp1, temp3, N);
  test (temp1, temp3, 0);
  check_memcpy64 (temp1, temp3, 0);

  reset_data64  (temp1, temp3, N);
  test (temp1, temp3, 1);
  check_memcpy64 (temp1, temp3, 1);

  reset_data64  (temp1, temp3, N);
  test (temp1, temp3, 2);
  check_memcpy64 (temp1, temp3, 2);

  reset_data64  (temp1, temp3, N);
  test (temp1, temp3, 3);
  check_memcpy64 (temp1, temp3, 3);

  reset_data64  (temp1, temp3, N);
  test (temp1, temp3, 4);
  check_memcpy64 (temp1, temp3, 4);

  reset_data64  (temp1, temp3, N);
  test (temp1, temp3, 5);
  check_memcpy64 (temp1, temp3, 5);

  reset_data64  (temp1, temp3, N);
  test (temp1, temp3, 6);
  check_memcpy64 (temp1, temp3, 6);

  reset_data64  (temp1, temp3, N);
  test (temp1, temp3, 7);
  check_memcpy64 (temp1, temp3, 7);

  reset_data64  (temp1, temp3, N);
}

/* { dg-final { scan-assembler-times {\tdlstp.64} 1 } } */
/* { dg-final { scan-assembler-times {\tletp} 1 } } */
/* { dg-final { scan-assembler-not "\tvctp" } } */
/* { dg-final { scan-assembler-not "\tvpst" } } */
/* { dg-final { scan-assembler-not "p0" } } */
