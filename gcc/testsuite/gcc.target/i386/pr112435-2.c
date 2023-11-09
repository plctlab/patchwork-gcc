/* PR target/112435 */
/* { dg-do compile { target { ! ia32 } } } */
/* { dg-options "-Ofast -march=sapphirerapids" } */
/* { dg-final { scan-assembler-not "vblendps.*ymm17\$" } } */

#include<x86intrin.h>

/* Vpermi128/Vpermf128 */
__m256i
perm0 (__m256i a, __m256i b)
{
  register __m256i t __asm__("ymm17") = a;
  asm("":"+v"(t));
  return _mm256_permute2x128_si256 (t, b, 50);
}

__m256i
perm1 (__m256i a, __m256i b)
{
  register __m256i t __asm__("ymm17") = a;
  asm("":"+v"(t));
  return _mm256_permute2x128_si256 (t, b, 18);
}

__m256i
perm2 (__m256i a, __m256i b)
{
  register __m256i t __asm__("ymm17") = a;
  asm("":"+v"(t));
  return _mm256_permute2x128_si256 (t, b, 48);
}

/* vshuf{i,f}{32x4,64x2} ymm .*/
__m256i
shuff0 (__m256i a, __m256i b)
{
  register __m256i t __asm__("ymm17") = a;
  asm("":"+v"(t));
  return _mm256_shuffle_i32x4(t, b, 2);
}

__m256
shuff1 (__m256 a, __m256 b)
{
  register __m256 t __asm__("ymm17") = a;
  asm("":"+v"(t));
  return _mm256_shuffle_f32x4(t, b, 2);
}

__m256i
shuff2 (__m256i a, __m256i b)
{
  register __m256i t __asm__("ymm17") = a;
  asm("":"+v"(t));
  return _mm256_shuffle_i64x2(t, b, 2);
}

__m256d
shuff3 (__m256d a, __m256d b)
{
  register __m256d t __asm__("ymm17") = a;
  asm("":"+v"(t));
  return _mm256_shuffle_f64x2(t, b, 2);
}
