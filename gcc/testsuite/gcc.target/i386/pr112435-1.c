/* PR target/112435 */
/* { dg-do compile { target { ! ia32 } } } */
/* { dg-options "-Ofast -march=sapphirerapids" } */
/* { dg-final { scan-assembler-not "vblendps" } } */

#include<x86intrin.h>

__m256i
f(__m256i a, __m256i  b)
{
  register __m256i t __asm__("ymm17") = a;
  asm("":"+v"(t));
  return _mm256_shuffle_i32x4 (t, b, 2);
}
