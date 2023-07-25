/* { dg-do compile } */
/* { dg-options "-mavx512f -O2" } */
/* { dg-final { scan-assembler "vpternlog.*0.*0.*0" } } */

#include <immintrin.h>

__m512i f(__m512i a, __m512i b, __m512i* c)
{
	return _mm512_ternarylogic_epi64 (a, b, c[0], 0x55);
}

