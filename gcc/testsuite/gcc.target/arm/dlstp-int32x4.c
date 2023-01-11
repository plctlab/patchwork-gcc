/* { dg-do run { target { arm*-*-* } } } */
/* { dg-require-effective-target arm_v8_1m_mve_ok } */
/* { dg-skip-if "avoid conflicting multilib options" { *-*-* } { "-marm" "-mcpu=*" } } */
/* { dg-options "-march=armv8.1-m.main+fp.dp+mve.fp -mfloat-abi=hard -mfpu=auto -O3 --save-temps" } */

#include <arm_mve.h>
#include <stdio.h>
#include <stdlib.h>
#include "lob.h"

void  __attribute__ ((noinline)) test (int32_t *a, int32_t *b, int32_t *c, int n)
{
  while (n > 0)
    {
      mve_pred16_t p = vctp32q (n);
      int32x4_t va = vldrwq_z_s32 (a, p);
      int32x4_t vb = vldrwq_z_s32 (b, p);
      int32x4_t vc = vaddq_x_s32 (va, vb, p);
      vstrwq_p_s32 (c, vc, p);
      c+=4;
      a+=4;
      b+=4;
      n-=4;
    }
}

int main ()
{
  int i;
  int32_t temp1[N];
  int32_t temp2[N];
  int32_t temp3[N];
  reset_data32 (temp1, temp2, temp3, N);
  test (temp1, temp2, temp3, 0);
  check_plus32 (temp1, temp2, temp3, 0);

  reset_data32 (temp1, temp2, temp3, N);
  test (temp1, temp2, temp3, 1);
  check_plus32 (temp1, temp2, temp3, 1);

  reset_data32 (temp1, temp2, temp3, N);
  test (temp1, temp2, temp3, 3);
  check_plus32 (temp1, temp2, temp3, 3);

  reset_data32 (temp1, temp2, temp3, N);
  test (temp1, temp2, temp3, 4);
  check_plus32 (temp1, temp2, temp3, 4);

  reset_data32 (temp1, temp2, temp3, N);
  test (temp1, temp2, temp3, 5);
  check_plus32 (temp1, temp2, temp3, 5);

  reset_data32 (temp1, temp2, temp3, N);
  test (temp1, temp2, temp3, 8);
  check_plus32 (temp1, temp2, temp3, 8);

  reset_data32 (temp1, temp2, temp3, N);
  test (temp1, temp2, temp3, 9);
  check_plus32 (temp1, temp2, temp3, 9);

  reset_data32 (temp1, temp2, temp3, N);
}

/* { dg-final { scan-assembler-times {\tdlstp.32} 1 } } */
/* { dg-final { scan-assembler-times {\tletp} 1 } } */
/* { dg-final { scan-assembler-not "\tvctp" } } */
/* { dg-final { scan-assembler-not "\tvpst" } } */
/* { dg-final { scan-assembler-not "p0" } } */
