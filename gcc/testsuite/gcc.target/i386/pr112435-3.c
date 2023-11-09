/* PR target/112435 */
/* { dg-do compile { target { ! ia32 } } } */
/* { dg-options "-Ofast -march=sapphirerapids" } */
/* { dg-final { scan-assembler-not "vblendps.*ymm17\$" } } */

#include<x86intrin.h>

/* Vpermf128 */
__m256
perm0 (__m256 a, __m256 b)
{
  register __m256 t __asm__("ymm17") =a;
  asm("":"+v"(t));
  return _mm256_permute2f128_ps (t, b, 50);
}

__m256
perm1 (__m256 a, __m256 b)
{
  register __m256 t __asm__("ymm17") =a;
  asm("":"+v"(t));
  return _mm256_permute2f128_ps (t, b, 18);
}

__m256
perm2 (__m256 a, __m256 b)
{
  register __m256 t __asm__("ymm17") =a;
  asm("":"+v"(t));
  return _mm256_permute2f128_ps (t, b, 48);
}

__m256i
perm3 (__m256i a, __m256i b)
{
  register __m256i t __asm__("ymm17") =a;
  asm("":"+v"(t));
  return _mm256_permute2f128_si256 (t, b, 50);
}

__m256i
perm4 (__m256i a, __m256i b)
{
  register __m256i t __asm__("ymm17") =a;
  asm("":"+v"(t));
  return _mm256_permute2f128_si256 (t, b, 18);
}

__m256i
perm5 (__m256i a, __m256i b)
{
  register __m256i t __asm__("ymm17") =a;
  asm("":"+v"(t));
  return _mm256_permute2f128_si256 (t, b, 48);
}

__m256d
perm6 (__m256d a, __m256d b)
{
  register __m256d t __asm__("ymm17") =a;
  asm("":"+v"(t));
  return _mm256_permute2f128_pd (t, b, 50);
}

__m256d
perm7 (__m256d a, __m256d b)
{
  register __m256d t __asm__("ymm17") =a;
  asm("":"+v"(t));
  return _mm256_permute2f128_pd (t, b, 18);
}

__m256d
perm8 (__m256d a, __m256d b)
{
  register __m256d t __asm__("ymm17") =a;
  asm("":"+v"(t));
  return _mm256_permute2f128_pd (t, b, 48);
}
