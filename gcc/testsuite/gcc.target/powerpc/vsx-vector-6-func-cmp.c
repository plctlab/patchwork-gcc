/* { dg-do run { target lp64 } } */
/* { dg-skip-if "" { powerpc*-*-darwin* } } */
/* { dg-options "-O2 -save-temps" } */

/* Functional test of the vector compare builtins.  */

#include <altivec.h>
#include <stdio.h>
#include <stdlib.h>

#define DEBUG 0

#define FLOAT_CHECK(NAME)                                                     \
  f_result = vec_##NAME (f_src_a, f_src_b);                                   \
                                                                              \
  if ((f_result[0] != f_##NAME##_expected[0])                                 \
      || (f_result[1] != f_##NAME##_expected[1])                              \
      || (f_result[2] != f_##NAME##_expected[2])                              \
      || (f_result[3] != f_##NAME##_expected[3]))                             \
    {                                                                         \
      if (DEBUG)                                                              \
        {                                                                     \
	  printf("ERROR: vec_%s (float) expected value does not match\n",     \
		 #NAME);                                                      \
	  printf("   expected[0] = 0x%x; result[0] =0x%x\n",                  \
		 f_cmpeq_expected[0], f_result[0]);                           \
	  printf("   expected[1] = 0x%x; result[1] = 0x%x\n",                 \
		 f_cmpeq_expected[1], f_result[1]);                           \
	  printf("   expected[2] = 0x%x; result[2] = 0x%x\n",                 \
		 f_cmpeq_expected[2], f_result[2]);                           \
	  printf("   expected[3] = 0x%x; result[3] = 0x%x\n",                 \
		 f_cmpeq_expected[3], f_result[3]);                           \
	}                                                                     \
      else                                                                    \
	abort();                                                              \
    }

#define DOUBLE_CHECK(NAME)                                                   \
  d_result = vec_##NAME (d_src_a, d_src_b);                                  \
                                                                             \
  if ((d_result[0] != d_##NAME##_expected[0])                                \
      || (d_result[1] != d_##NAME##_expected[1]))                            \
    {                                                                        \
      if (DEBUG)                                                             \
	{                                                                    \
	  printf("ERROR: vec_%s (double) expected value does not match\n",   \
		 #NAME);                                                     \
	  printf("   expected[0] = 0x%lx; result[0] = 0x%lx\n",              \
		 d_cmpeq_expected[0], d_result[0]);                          \
	  printf("   expected[1] = 0x%lx; result[1] = 0x%lx\n",              \
		 d_cmpeq_expected[1], d_result[1]);                          \
	}                                                                    \
      else                                                                   \
	abort();                                                             \
    }

void abort (void);

void float_tests (vector float f_src_a, vector float f_src_b,
		  vector bool int f_cmpeq_expected,
		  vector bool int f_cmpgt_expected,
		  vector bool int f_cmpge_expected,
		  vector bool int f_cmplt_expected,
		  vector bool int f_cmple_expected)
{
  /* The tests are simple enough that the compiler computes the answer at
     compile time.  Put the tests in a function so the compiler will generate
     the code needed to perform the test.  */
  vector bool int f_result;

  FLOAT_CHECK (cmpeq)
  FLOAT_CHECK (cmpgt)
  FLOAT_CHECK (cmpge)
  FLOAT_CHECK (cmplt)
  FLOAT_CHECK (cmple)
}

void double_tests (vector double d_src_a, vector double d_src_b,
		   vector bool long long d_cmpeq_expected,
		   vector bool long long d_cmpgt_expected,
		   vector bool long long d_cmpge_expected,
		   vector bool long long d_cmplt_expected,
		   vector bool long long d_cmple_expected)
{
  /* The tests are simple enough that the compiler computes the answer at
     compile time.  Put the tests in a function so the compiler will generate
     the code needed to perform the test.  */
  vector bool long long d_result;

  DOUBLE_CHECK (cmpeq)
  DOUBLE_CHECK (cmpgt)
  DOUBLE_CHECK (cmpge)
  DOUBLE_CHECK (cmplt)
  DOUBLE_CHECK (cmple)
}

int
main () {
  int i;
  vector float f_src_a = { 126.0, 23.0, -338.0, 17.0};
  vector float f_src_b = { 2.00, 23.0, 1.0, 4.0};
  vector bool int f_cmpeq_expected = {0x0, 0xFFFFFFFF, 0x0, 0x0};
  vector bool int f_cmpgt_expected = {0xFFFFFFFF, 0x0, 0x0, 0xFFFFFFFF};
  vector bool int f_cmpge_expected = {0xFFFFFFFF, 0xFFFFFFFF, 0x0, 0xFFFFFFFF};
  vector bool int f_cmplt_expected = {0x0, 0x0, 0xFFFFFFFF, 0x0}; 
  vector bool int f_cmple_expected = {0x0, 0xFFFFFFFF, 0xFFFFFFFF, 0x0}; 

  vector double d_src_a = { 125.44, -338.56};
  vector double d_src_b = { 4.0, -338.56};
  vector bool long long d_cmpeq_expected = {0x0, 0xFFFFFFFFFFFFFFFF};
  vector bool long long d_cmpgt_expected = {0xFFFFFFFFFFFFFFFF, 0x0};
  vector bool long long d_cmpge_expected = {0xFFFFFFFFFFFFFFFF,
					    0xFFFFFFFFFFFFFFFF};
  vector bool long long d_cmplt_expected = {0x0, 0x0};
  vector bool long long d_cmple_expected = {0x0, 0xFFFFFFFFFFFFFFFF};

  float_tests (f_src_a, f_src_b, f_cmpeq_expected, f_cmpgt_expected,
	       f_cmpge_expected, f_cmplt_expected, f_cmple_expected);
 
  double_tests (d_src_a, d_src_b, d_cmpeq_expected, d_cmpgt_expected,
		d_cmpge_expected, d_cmplt_expected, d_cmple_expected);
  return 0;
}

/* { dg-final { scan-assembler-times {\mxvcmpeqsp\M} 1 } } */
/* { dg-final { scan-assembler-times {\mxvcmpgtsp\M} 2 } } */
/* { dg-final { scan-assembler-times {\mxvcmpgesp\M} 2 } } */
/* { dg-final { scan-assembler-times {\mxvcmpeqdp\M} 1 } } */
/* { dg-final { scan-assembler-times {\mxvcmpgedp\M} 2 } } */
/* { dg-final { scan-assembler-times {\mxvcmpgtdp\M} 2 } } */
