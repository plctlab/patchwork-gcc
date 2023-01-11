
/* { dg-do compile { target { arm*-*-* } } } */
/* { dg-require-effective-target arm_v8_1m_mve_ok } */
/* { dg-skip-if "avoid conflicting multilib options" { *-*-* } { "-marm" "-mcpu=*" } } */
/* { dg-options "-march=armv8.1-m.main+fp.dp+mve.fp -mfloat-abi=hard -mfpu=auto -O3 --save-temps" } */

#include <arm_mve.h>

/* Terminating on a non-zero number of elements.  */
void test1 (uint8_t *a, uint8_t *b, uint8_t *c, int n)
{
    while (n > 1)
    {
        mve_pred16_t p = vctp8q (n);
        uint8x16_t va = vldrbq_z_u8 (a, p);
        uint8x16_t vb = vldrbq_z_u8 (b, p);
        uint8x16_t vc = vaddq_x_u8 (va, vb, p);
        vstrbq_p_u8 (c, vc, p);
        n -= 16;
    }
}

/* Similar, terminating on a non-zero number of elements, but in a for loop
   format.  */
int32_t a[] = {0, 1, 2, 3, 4, 5, 6, 7};
void test2 (int32_t *b, int num_elems)
{
    for (int i = num_elems; i >= 2; i-= 4)
    {
        mve_pred16_t p = vctp32q (i);
        int32x4_t va = vldrwq_z_s32 (&(a[i]), p);
        vstrwq_p_s32 (b + i, va, p);
    }
}

/* Iteration counter counting up to num_iter, with a non-zero starting num.  */
void test3 (uint8_t *a, uint8_t *b, uint8_t *c, int n)
{
    int num_iter = (n + 15)/16;
    for (int i = 1; i < num_iter; i++)
    {
        mve_pred16_t p = vctp8q (n);
        uint8x16_t va = vldrbq_z_u8 (a, p);
        uint8x16_t vb = vldrbq_z_u8 (b, p);
        uint8x16_t vc = vaddq_x_u8 (va, vb, p);
        vstrbq_p_u8 (c, vc, p);
        n -= 16;
    }
}

/* Iteration counter counting up to num_iter, with a larger increment  */
void test4 (uint8_t *a, uint8_t *b, uint8_t *c, int n)
{
    int num_iter = (n + 15)/16;
    for (int i = 0; i < num_iter; i+=2)
    {
        mve_pred16_t p = vctp8q (n);
        uint8x16_t va = vldrbq_z_u8 (a, p);
        uint8x16_t vb = vldrbq_z_u8 (b, p);
        uint8x16_t vc = vaddq_x_u8 (va, vb, p);
        vstrbq_p_u8 (c, vc, p);
        n -= 16;
    }
}

/* Using an unpredicated store instruction within the loop.  */
void test5 (uint8_t *a, uint8_t *b, uint8_t *c,  uint8_t *d, int n)
{
    while (n > 0)
    {
        mve_pred16_t p = vctp8q (n);
        uint8x16_t va = vldrbq_z_u8 (a, p);
        uint8x16_t vb = vldrbq_z_u8 (b, p);
        uint8x16_t vc = vaddq_u8 (va, vb);
        uint8x16_t vd = vaddq_x_u8 (va, vb, p);
        vstrbq_u8 (d, vd);
        n -= 16;
    }
}

/* Using an unpredicated store outside the loop.  */
void test16 (uint8_t *a, uint8_t *b, uint8_t *c, int n, uint8x16_t vx)
{
    uint8_t sum = 0;
    while (n > 0)
    {
        mve_pred16_t p = vctp8q (n);
        uint8x16_t va = vldrbq_z_u8 (a, p);
        uint8x16_t vb = vldrbq_z_u8 (b, p);
        uint8x16_t vc = vaddq_m_u8 (vx, va, vb, p);
        vx = vaddq_u8 (vx, vc);
        a += 16;
        b += 16;
        n -= 16;
    }
    vstrbq_u8 (c, vx);
}

/* Using an unpredicated op where the result is valid outside the bb.  */
uint8_t test17 (uint8_t *a, uint8_t *b, uint8_t *c, int n, uint8x16_t vx)
{
    uint8_t sum = 0;
    while (n > 0)
    {
        mve_pred16_t p = vctp8q (n);
        uint8x16_t va = vldrbq_z_u8 (a, p);
        uint8x16_t vb = vldrbq_z_u8 (b, p);
        uint8x16_t vc = vaddq_m_u8 (vx, va, vb, p);
        sum += vaddvq_u8 (vc);
        a += 16;
        b += 16;
        n -= 16;
    }
    return sum;
}

/* Using a VPR that gets modified within the loop.  */
void test7 (int32_t *a, int32_t *b, int32_t *c, int n)
{
  while (n > 0)
    {
      mve_pred16_t p = vctp32q (n);
      int32x4_t va = vldrwq_z_s32 (a, p);
      p++;
      int32x4_t vb = vldrwq_z_s32 (b, p);
      int32x4_t vc = vaddq_x_s32 (va, vb, p);
      vstrwq_p_s32 (c, vc, p);
      c += 4;
      a += 4;
      b += 4;
      n -= 4;
    }
}

/* Using a VPR that gets re-generated within the loop.  */
void test8 (int32_t *a, int32_t *b, int32_t *c, int n)
{
  mve_pred16_t p = vctp32q (n);
  while (n > 0)
    {
      int32x4_t va = vldrwq_z_s32 (a, p);
      p = vctp32q (n);
      int32x4_t vb = vldrwq_z_s32 (b, p);
      int32x4_t vc = vaddq_x_s32 (va, vb, p);
      vstrwq_p_s32 (c, vc, p);
      c += 4;
      a += 4;
      b += 4;
      n -= 4;
    }
}

/* Using vctp32q_m instead of vctp32q.  */
void test9 (int32_t *a, int32_t *b, int32_t *c, int n, mve_pred16_t p0)
{
  while (n > 0)
    {
      mve_pred16_t p = vctp32q_m (n, p0);
      int32x4_t va = vldrwq_z_s32 (a, p);
      int32x4_t vb = vldrwq_z_s32 (b, p);
      int32x4_t vc = vaddq_x_s32 (va, vb, p);
      vstrwq_p_s32 (c, vc, p);
      c += 4;
      a += 4;
      b += 4;
      n -= 4;
    }
}

/* Generating and using a different VPR value in the loop, with a vctp_m that is tied to the base vctp VPR.  */
void test10 (int32_t *a, int32_t *b, int32_t *c, int n)
{
  while (n > 0)
    {
      mve_pred16_t p = vctp32q (n);
      int32x4_t va = vldrwq_z_s32 (a, p);
      mve_pred16_t p1 = vctp32q_m (n, p);
      int32x4_t vb = vldrwq_z_s32 (b, p1);
      int32x4_t vc = vaddq_x_s32 (va, vb, p1);
      vstrwq_p_s32 (c, vc, p);
      c += 4;
      a += 4;
      b += 4;
      n -= 4;
    }
}

/* { dg-final { scan-assembler-not "\tdlstp" } } */
/* { dg-final { scan-assembler-not "\tletp" } } */
