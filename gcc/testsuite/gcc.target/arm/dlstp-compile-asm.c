/* { dg-do compile { target { arm*-*-* } } } */
/* { dg-require-effective-target arm_v8_1m_mve_ok } */
/* { dg-skip-if "avoid conflicting multilib options" { *-*-* } { "-marm" "-mcpu=*" } } */
/* { dg-options "-march=armv8.1-m.main+fp.dp+mve.fp -mfloat-abi=hard -mfpu=auto -O3" } */

#include <arm_mve.h>

#define IMM 5

#define TEST_COMPILE_IN_DLSTP_TERNARY(BITS, LANES, LDRSTRYTPE, TYPE, SIGN, NAME, PRED)				\
void test_##NAME##PRED##_##SIGN##BITS (TYPE##BITS##_t *a, TYPE##BITS##_t *b,  TYPE##BITS##_t *c, int n)	\
{											\
  while (n > 0)										\
    {											\
      mve_pred16_t p = vctp##BITS##q (n);						\
      TYPE##BITS##x##LANES##_t va = vldr##LDRSTRYTPE##q_z_##SIGN##BITS (a, p);		\
      TYPE##BITS##x##LANES##_t vb = vldr##LDRSTRYTPE##q_z_##SIGN##BITS (b, p);		\
      TYPE##BITS##x##LANES##_t vc = NAME##PRED##_##SIGN##BITS (va, vb, p);		\
      vstr##LDRSTRYTPE##q_p_##SIGN##BITS (c, vc, p);					\
      c += LANES;									\
      a += LANES;									\
      b += LANES;									\
      n -= LANES;									\
    }											\
}

#define TEST_COMPILE_IN_DLSTP_SIGNED_UNSIGNED_TERNARY(BITS, LANES, LDRSTRYTPE, NAME, PRED)	\
TEST_COMPILE_IN_DLSTP_TERNARY (BITS, LANES, LDRSTRYTPE, int, s, NAME, PRED)			\
TEST_COMPILE_IN_DLSTP_TERNARY (BITS, LANES, LDRSTRYTPE, uint, u, NAME, PRED)

#define TEST_COMPILE_IN_DLSTP_INTBITS_SIGNED_UNSIGNED_TERNARY(NAME, PRED)			\
TEST_COMPILE_IN_DLSTP_SIGNED_UNSIGNED_TERNARY (8, 16, b, NAME, PRED)				\
TEST_COMPILE_IN_DLSTP_SIGNED_UNSIGNED_TERNARY (16, 8, h, NAME, PRED)				\
TEST_COMPILE_IN_DLSTP_SIGNED_UNSIGNED_TERNARY (32, 4, w, NAME, PRED)


TEST_COMPILE_IN_DLSTP_INTBITS_SIGNED_UNSIGNED_TERNARY (vaddq, _x)
TEST_COMPILE_IN_DLSTP_INTBITS_SIGNED_UNSIGNED_TERNARY (vmulq, _x)
TEST_COMPILE_IN_DLSTP_INTBITS_SIGNED_UNSIGNED_TERNARY (vsubq, _x)
TEST_COMPILE_IN_DLSTP_INTBITS_SIGNED_UNSIGNED_TERNARY (vhaddq, _x)
TEST_COMPILE_IN_DLSTP_INTBITS_SIGNED_UNSIGNED_TERNARY (vorrq, _x)


#define TEST_COMPILE_IN_DLSTP_TERNARY_M(BITS, LANES, LDRSTRYTPE, TYPE, SIGN, NAME, PRED)				\
void test_##NAME##PRED##_##SIGN##BITS (TYPE##BITS##x##LANES##_t __inactive, TYPE##BITS##_t *a, TYPE##BITS##_t *b,  TYPE##BITS##_t *c, int n)	\
{											\
  while (n > 0)										\
    {											\
      mve_pred16_t p = vctp##BITS##q (n);						\
      TYPE##BITS##x##LANES##_t va = vldr##LDRSTRYTPE##q_z_##SIGN##BITS (a, p);		\
      TYPE##BITS##x##LANES##_t vb = vldr##LDRSTRYTPE##q_z_##SIGN##BITS (b, p);		\
      TYPE##BITS##x##LANES##_t vc = NAME##PRED##_##SIGN##BITS (__inactive, va, vb, p);		\
      vstr##LDRSTRYTPE##q_p_##SIGN##BITS (c, vc, p);					\
      c += LANES;									\
      a += LANES;									\
      b += LANES;									\
      n -= LANES;									\
    }											\
}

#define TEST_COMPILE_IN_DLSTP_SIGNED_UNSIGNED_TERNARY_M(BITS, LANES, LDRSTRYTPE, NAME, PRED)	\
TEST_COMPILE_IN_DLSTP_TERNARY_M (BITS, LANES, LDRSTRYTPE, int, s, NAME, PRED)			\
TEST_COMPILE_IN_DLSTP_TERNARY_M (BITS, LANES, LDRSTRYTPE, uint, u, NAME, PRED)

#define TEST_COMPILE_IN_DLSTP_INTBITS_SIGNED_UNSIGNED_TERNARY_M(NAME, PRED)			\
TEST_COMPILE_IN_DLSTP_SIGNED_UNSIGNED_TERNARY_M (8, 16, b, NAME, PRED)				\
TEST_COMPILE_IN_DLSTP_SIGNED_UNSIGNED_TERNARY_M (16, 8, h, NAME, PRED)				\
TEST_COMPILE_IN_DLSTP_SIGNED_UNSIGNED_TERNARY_M (32, 4, w, NAME, PRED)


TEST_COMPILE_IN_DLSTP_INTBITS_SIGNED_UNSIGNED_TERNARY_M (vaddq, _m)
TEST_COMPILE_IN_DLSTP_INTBITS_SIGNED_UNSIGNED_TERNARY_M (vmulq, _m)
TEST_COMPILE_IN_DLSTP_INTBITS_SIGNED_UNSIGNED_TERNARY_M (vsubq, _m)
TEST_COMPILE_IN_DLSTP_INTBITS_SIGNED_UNSIGNED_TERNARY_M (vhaddq, _m)
TEST_COMPILE_IN_DLSTP_INTBITS_SIGNED_UNSIGNED_TERNARY_M (vorrq, _m)

#define TEST_COMPILE_IN_DLSTP_TERNARY_N(BITS, LANES, LDRSTRYTPE, TYPE, SIGN, NAME, PRED)	\
void test_##NAME##PRED##_n_##SIGN##BITS (TYPE##BITS##_t *a,  TYPE##BITS##_t *c, int n)	\
{											\
  while (n > 0)										\
    {											\
      mve_pred16_t p = vctp##BITS##q (n);						\
      TYPE##BITS##x##LANES##_t va = vldr##LDRSTRYTPE##q_z_##SIGN##BITS (a, p);		\
      TYPE##BITS##x##LANES##_t vc = NAME##PRED##_n_##SIGN##BITS (va, IMM, p);		\
      vstr##LDRSTRYTPE##q_p_##SIGN##BITS (c, vc, p);					\
      c += LANES;									\
      a += LANES;									\
      n -= LANES;									\
    }											\
}

#define TEST_COMPILE_IN_DLSTP_SIGNED_UNSIGNED_TERNARY_N(BITS, LANES, LDRSTRYTPE, NAME, PRED)	\
TEST_COMPILE_IN_DLSTP_TERNARY_N (BITS, LANES, LDRSTRYTPE, int, s, NAME, PRED)			\
TEST_COMPILE_IN_DLSTP_TERNARY_N (BITS, LANES, LDRSTRYTPE, uint, u, NAME, PRED)

#define TEST_COMPILE_IN_DLSTP_INTBITS_SIGNED_UNSIGNED_TERNARY_N(NAME, PRED)			\
TEST_COMPILE_IN_DLSTP_SIGNED_UNSIGNED_TERNARY_N (8, 16, b, NAME, PRED)				\
TEST_COMPILE_IN_DLSTP_SIGNED_UNSIGNED_TERNARY_N (16, 8, h, NAME, PRED)				\
TEST_COMPILE_IN_DLSTP_SIGNED_UNSIGNED_TERNARY_N (32, 4, w, NAME, PRED)

TEST_COMPILE_IN_DLSTP_INTBITS_SIGNED_UNSIGNED_TERNARY_N (vaddq, _x)
TEST_COMPILE_IN_DLSTP_INTBITS_SIGNED_UNSIGNED_TERNARY_N (vmulq, _x)
TEST_COMPILE_IN_DLSTP_INTBITS_SIGNED_UNSIGNED_TERNARY_N (vsubq, _x)
TEST_COMPILE_IN_DLSTP_INTBITS_SIGNED_UNSIGNED_TERNARY_N (vhaddq, _x)

TEST_COMPILE_IN_DLSTP_INTBITS_SIGNED_UNSIGNED_TERNARY_N (vbrsrq, _x)
TEST_COMPILE_IN_DLSTP_INTBITS_SIGNED_UNSIGNED_TERNARY_N (vshlq, _x)
TEST_COMPILE_IN_DLSTP_INTBITS_SIGNED_UNSIGNED_TERNARY_N (vshrq, _x)

#define TEST_COMPILE_IN_DLSTP_TERNARY_M_N(BITS, LANES, LDRSTRYTPE, TYPE, SIGN, NAME, PRED)	\
void test_##NAME##PRED##_n_##SIGN##BITS (TYPE##BITS##x##LANES##_t __inactive, TYPE##BITS##_t *a,  TYPE##BITS##_t *c, int n)	\
{											\
  while (n > 0)										\
    {											\
      mve_pred16_t p = vctp##BITS##q (n);						\
      TYPE##BITS##x##LANES##_t va = vldr##LDRSTRYTPE##q_z_##SIGN##BITS (a, p);		\
      TYPE##BITS##x##LANES##_t vc = NAME##PRED##_n_##SIGN##BITS (__inactive, va, IMM, p);		\
      vstr##LDRSTRYTPE##q_p_##SIGN##BITS (c, vc, p);					\
      c += LANES;									\
      a += LANES;									\
      n -= LANES;									\
    }											\
}

#define TEST_COMPILE_IN_DLSTP_SIGNED_UNSIGNED_TERNARY_M_N(BITS, LANES, LDRSTRYTPE, NAME, PRED)	\
TEST_COMPILE_IN_DLSTP_TERNARY_M_N (BITS, LANES, LDRSTRYTPE, int, s, NAME, PRED)			\
TEST_COMPILE_IN_DLSTP_TERNARY_M_N (BITS, LANES, LDRSTRYTPE, uint, u, NAME, PRED)

#define TEST_COMPILE_IN_DLSTP_INTBITS_SIGNED_UNSIGNED_TERNARY_M_N(NAME, PRED)			\
TEST_COMPILE_IN_DLSTP_SIGNED_UNSIGNED_TERNARY_M_N (8, 16, b, NAME, PRED)				\
TEST_COMPILE_IN_DLSTP_SIGNED_UNSIGNED_TERNARY_M_N (16, 8, h, NAME, PRED)				\
TEST_COMPILE_IN_DLSTP_SIGNED_UNSIGNED_TERNARY_M_N (32, 4, w, NAME, PRED)

TEST_COMPILE_IN_DLSTP_INTBITS_SIGNED_UNSIGNED_TERNARY_M_N (vaddq, _m)
TEST_COMPILE_IN_DLSTP_INTBITS_SIGNED_UNSIGNED_TERNARY_M_N (vmulq, _m)
TEST_COMPILE_IN_DLSTP_INTBITS_SIGNED_UNSIGNED_TERNARY_M_N (vsubq, _m)
TEST_COMPILE_IN_DLSTP_INTBITS_SIGNED_UNSIGNED_TERNARY_M_N (vhaddq, _m)

TEST_COMPILE_IN_DLSTP_INTBITS_SIGNED_UNSIGNED_TERNARY_M_N (vbrsrq, _m)
TEST_COMPILE_IN_DLSTP_INTBITS_SIGNED_UNSIGNED_TERNARY_M_N (vshlq, _m)
TEST_COMPILE_IN_DLSTP_INTBITS_SIGNED_UNSIGNED_TERNARY_M_N (vshrq, _m)

/* Now test some more configurations.  */

/* Test a for loop format of decrementing to zero */
int32_t a[] = {0, 1, 2, 3, 4, 5, 6, 7};
void test1 (int32_t *b, int num_elems)
{
    for (int i = num_elems; i >= 0; i-= 4)
    {
        mve_pred16_t p = vctp32q (i);
        int32x4_t va = vldrwq_z_s32 (&(a[i]), p);
        vstrwq_p_s32 (b + i, va, p);
    }
}

/* Iteration counter counting up to num_iter.  */
void test2 (uint8_t *a, uint8_t *b, uint8_t *c, int n)
{
    int num_iter = (n + 15)/16;
    for (int i = 0; i < num_iter; i++)
    {
        mve_pred16_t p = vctp8q (n);
        uint8x16_t va = vldrbq_z_u8 (a, p);
        uint8x16_t vb = vldrbq_z_u8 (b, p);
        uint8x16_t vc = vaddq_x_u8 (va, vb, p);
        vstrbq_p_u8 (c, vc, p);
        n-=16;
    }
}

/* Using an unpredicated arithmetic instruction within the loop.  */
void test5 (uint8_t *a, uint8_t *b, uint8_t *c,  uint8_t *d, int n)
{
    while (n > 0)
    {
        mve_pred16_t p = vctp8q (n);
        uint8x16_t va = vldrbq_z_u8 (a, p);
        uint8x16_t vb = vldrbq_u8 (b);
        uint8x16_t vc = vaddq_u8 (va, vb);
        uint8x16_t vd = vaddq_x_u8 (va, vb, p);
        vstrbq_p_u8 (c, vc, p);
        vstrbq_p_u8 (d, vd, p);
        n-=16;
    }
}

/* Using a different VPR value for one instruction in the loop.  */
void test11 (int32_t *a, int32_t *b, int32_t *c, int n, mve_pred16_t p1)
{
  while (n > 0)
    {
      mve_pred16_t p = vctp32q (n);
      int32x4_t va = vldrwq_z_s32 (a, p);
      int32x4_t vb = vldrwq_z_s32 (b, p1);
      int32x4_t vc = vaddq_x_s32 (va, vb, p);
      vstrwq_p_s32 (c, vc, p);
      c += 4;
      a += 4;
      b += 4;
      n -= 4;
    }
}



/* Generating and using a constant VPR value in the loop, with a vctp.  */
void test12 (int32_t *a, int32_t *b, int32_t *c, int n, int g)
{
  while (n > 0)
    {
      mve_pred16_t p = vctp32q (n);
      int32x4_t va = vldrwq_z_s32 (a, p);
      mve_pred16_t p1 = vctp32q (g);
      int32x4_t vb = vldrwq_z_s32 (b, p1);
      int32x4_t vc = vaddq_x_s32 (va, vb, p);
      vstrwq_p_s32 (c, vc, p);
      c += 4;
      a += 4;
      b += 4;
      n -= 4;
    }
}

/* Generating and using a different VPR value in the loop, with a vctp.  */
void test122 (int32_t *a, int32_t *b, int32_t *c, int n, int g)
{
  while (n > 0)
    {
      mve_pred16_t p = vctp32q (n);
      int32x4_t va = vldrwq_z_s32 (a, p);
      mve_pred16_t p1 = vctp32q (g);
      int32x4_t vb = vldrwq_z_s32 (b, p1);
      int32x4_t vc = vaddq_x_s32 (va, vb, p);
      vstrwq_p_s32 (c, vc, p);
      c += 4;
      a += 4;
      b += 4;
      n -= 4;
      g++;
    }
}

/* Generating and using a different VPR value in the loop, with a vctp_m.  */
void test13 (int32_t *a, int32_t *b, int32_t *c, int n, mve_pred16_t p1)
{
  while (n > 0)
    {
      mve_pred16_t p = vctp32q (n);
      int32x4_t va = vldrwq_z_s32 (a, p);
      mve_pred16_t p2 = vctp32q_m (n, p1);
      int32x4_t vb = vldrwq_z_s32 (b, p1);
      int32x4_t vc = vaddq_x_s32 (va, vb, p2);
      vstrwq_p_s32 (c, vc, p);
      c += 4;
      a += 4;
      b += 4;
      n -= 4;
    }
}

/* Generating and using a different VPR value in the loop, with a vcmp.  */
void test14 (int32_t *a, int32_t *b, int32_t *c, int n)
{
  while (n > 0)
    {
      mve_pred16_t p = vctp32q (n);
      int32x4_t va = vldrwq_z_s32 (a, p);
      int32x4_t vb = vldrwq_z_s32 (b, p);
      mve_pred16_t p1 = vcmpeqq_s32 (va, vb);
      int32x4_t vc = vaddq_x_s32 (va, vb, p1);
      vstrwq_p_s32 (c, vc, p);
      c += 4;
      a += 4;
      b += 4;
      n -= 4;
    }
}

/* Generating and using a different VPR value in the loop, with a vcmp_m.  */
void test15 (int32_t *a, int32_t *b, int32_t *c, int n, mve_pred16_t p1)
{
  while (n > 0)
    {
      mve_pred16_t p = vctp32q (n);
      int32x4_t va = vldrwq_z_s32 (a, p);
      int32x4_t vb = vldrwq_z_s32 (b, p);
      mve_pred16_t p2 = vcmpeqq_m_s32 (va, vb, p1);
      int32x4_t vc = vaddq_x_s32 (va, vb, p2);
      vstrwq_p_s32 (c, vc, p);
      c += 4;
      a += 4;
      b += 4;
      n -= 4;
    }
}

/* The final number of DLSTPs currently is calculated by the number of
  `TEST_COMPILE_IN_DLSTP_INTBITS_SIGNED_UNSIGNED_TERNARY.*` macros * 6 + 9.  */
/* { dg-final { scan-assembler-times {\tdlstp} 153 } } */
/* { dg-final { scan-assembler-times {\tletp} 153 } } */
