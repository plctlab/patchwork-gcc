/* { dg-do run { target { arm*-*-* } } } */
/* { dg-require-effective-target arm_v8_1m_mve_ok } */
/* { dg-skip-if "avoid conflicting multilib options" { *-*-* } { "-marm" "-mcpu=*" } } */
/* { dg-options "-march=armv8.1-m.main+fp.dp+mve.fp -mfloat-abi=hard -mfpu=auto -O3 --save-temps" } */

#include <arm_mve.h>
#include <stdio.h>
#include <stdlib.h>
#include "lob.h"

void  __attribute__ ((noinline)) test (int16_t *a, int16_t *b, int16_t *c, int n)
{
  while (n > 0)
    {
      mve_pred16_t p = vctp16q (n);
      int16x8_t va = vldrhq_z_s16 (a, p);
      int16x8_t vb = vldrhq_z_s16 (b, p);
      int16x8_t vc = vaddq_x_s16 (va, vb, p);
      vstrhq_p_s16 (c, vc, p);
      c+=8;
      a+=8;
      b+=8;
      n-=8;
    }
}

int main ()
{
  int i;
  int16_t temp1[N];
  int16_t temp2[N];
  int16_t temp3[N];
  reset_data16 (temp1, temp2, temp3, N);
  test (temp1, temp2, temp3, 0);
  check_plus16 (temp1, temp2, temp3, 0);

  reset_data16 (temp1, temp2, temp3, N);
  test (temp1, temp2, temp3, 1);
  check_plus16 (temp1, temp2, temp3, 1);

  reset_data16 (temp1, temp2, temp3, N);
  test (temp1, temp2, temp3, 7);
  check_plus16 (temp1, temp2, temp3, 7);

  reset_data16 (temp1, temp2, temp3, N);
  test (temp1, temp2, temp3, 8);
  check_plus16 (temp1, temp2, temp3, 8);

  reset_data16 (temp1, temp2, temp3, N);
  test (temp1, temp2, temp3, 9);
  check_plus16 (temp1, temp2, temp3, 9);

  reset_data16 (temp1, temp2, temp3, N);
  test (temp1, temp2, temp3, 16);
  check_plus16 (temp1, temp2, temp3, 16);

  reset_data16 (temp1, temp2, temp3, N);
  test (temp1, temp2, temp3, 17);
  check_plus16 (temp1, temp2, temp3, 17);

  reset_data16 (temp1, temp2, temp3, N);
}

/* { dg-final { scan-assembler-times {\tdlstp.16} 1 } } */
/* { dg-final { scan-assembler-times {\tletp} 1 } } */
/* { dg-final { scan-assembler-not "\tvctp" } } */
/* { dg-final { scan-assembler-not "\tvpst" } } */
/* { dg-final { scan-assembler-not "p0" } } */
