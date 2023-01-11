/* { dg-do run { target { arm*-*-* } } } */
/* { dg-require-effective-target arm_v8_1m_mve_ok } */
/* { dg-skip-if "avoid conflicting multilib options" { *-*-* } { "-marm" "-mcpu=*" } } */
/* { dg-options "-march=armv8.1-m.main+fp.dp+mve.fp -mfloat-abi=hard -mfpu=auto -O3 --save-temps" } */

#include <arm_mve.h>
#include <stdio.h>
#include <stdlib.h>
#include "lob.h"

void  __attribute__ ((noinline)) test (int8_t *a, int8_t *b, int8_t *c, int n)
{
  while (n > 0)
    {
      mve_pred16_t p = vctp8q (n);
      int8x16_t va = vldrbq_z_s8 (a, p);
      int8x16_t vb = vldrbq_z_s8 (b, p);
      int8x16_t vc = vaddq_x_s8 (va, vb, p);
      vstrbq_p_s8 (c, vc, p);
      c+=16;
      a+=16;
      b+=16;
      n-=16;
    }
}

int main ()
{
  int i;
  int8_t temp1[N];
  int8_t temp2[N];
  int8_t temp3[N];
  reset_data8 (temp1, temp2, temp3, N);
  test (temp1, temp2, temp3, 0);
  check_plus8 (temp1, temp2, temp3, 0);

  reset_data8 (temp1, temp2, temp3, N);
  test (temp1, temp2, temp3, 1);
  check_plus8 (temp1, temp2, temp3, 1);

  reset_data8 (temp1, temp2, temp3, N);
  test (temp1, temp2, temp3, 15);
  check_plus8 (temp1, temp2, temp3, 15);

  reset_data8 (temp1, temp2, temp3, N);
  test (temp1, temp2, temp3, 16);
  check_plus8 (temp1, temp2, temp3, 16);

  reset_data8 (temp1, temp2, temp3, N);
  test (temp1, temp2, temp3, 17);
  check_plus8 (temp1, temp2, temp3, 17);

  reset_data8 (temp1, temp2, temp3, N);
  test (temp1, temp2, temp3, 32);
  check_plus8 (temp1, temp2, temp3, 32);

  reset_data8 (temp1, temp2, temp3, N);
  test (temp1, temp2, temp3, 33);
  check_plus8 (temp1, temp2, temp3, 33);

  reset_data8 (temp1, temp2, temp3, N);
}

/* { dg-final { scan-assembler-times {\tdlstp.8} 1 } } */
/* { dg-final { scan-assembler-times {\tletp} 1 } } */
/* { dg-final { scan-assembler-not "\tvctp" } } */
/* { dg-final { scan-assembler-not "\tvpst" } } */
/* { dg-final { scan-assembler-not "p0" } } */
