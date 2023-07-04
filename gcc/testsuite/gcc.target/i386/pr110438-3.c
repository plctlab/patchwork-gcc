/*  PR target/110438 generating all-ones zmm needs dep-breaking pxor before ternlog */
/* { dg-do compile } */
/* { dg-options "-mavx512f -Os" } */

typedef int v64 __attribute__((vector_size(64)));

v64 g(void)
{
	return (v64){0} - 1;
}

/* { dg-final { scan-assembler-not "vpxor" } }*/
