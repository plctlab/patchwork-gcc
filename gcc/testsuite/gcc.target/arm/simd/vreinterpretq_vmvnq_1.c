/* { dg-do compile } */
/* { dg-options "-O2" } */
/* { dg-additional-options "-march=armv8.2-a -mfloat-abi=hard -mfpu=neon" } */

#include <arm_neon.h>

int64x2_t test_vector1(int32x4_t a, int32x4_t b)
{
  return vandq_s64(vreinterpretq_s64_s32(vmvnq_s32(a)),
                   vreinterpretq_s64_s32(b));
}

int64x2_t test_vector2(int32x4_t a, int16x8_t b)
{
  return vandq_s64(vreinterpretq_s64_s32(vmvnq_s32(a)),
                   vreinterpretq_s64_s16(b));
}

int64x2_t test_vector3(int32x4_t a, int64x2_t b)
{
  return vandq_s64(vreinterpretq_s64_s32(vmvnq_s32(a)), b);
}

/* { dg-final { scan-assembler-times {\tvbic\t} 3 } } */
/* { dg-final { scan-assembler-not {\tvand\t} } } */
/* { dg-final { scan-assembler-not {\tvmvn\t} } } */
