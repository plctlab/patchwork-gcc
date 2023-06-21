/* { dg-do run { target lp64 } } */
/* { dg-skip-if "" { powerpc*-*-darwin* } } */
/* { dg-options "-O2 -save-temps" } */
/* Functional test of the two operand vector builtins.  */

#include <altivec.h>
#include <stdio.h>
#include <stdlib.h>

#define DEBUG 0

#define FLOAT_CHECK(NAME)						   \
  f_result = vec_##NAME (f_src_a, f_src_b);				   \
                                                                           \
  if ((f_result[0] != f_##NAME##_expected[0])				   \
      || (f_result[1] != f_##NAME##_expected[1])                           \
      || (f_result[2] != f_##NAME##_expected[2])                           \
      || (f_result[3] != f_##NAME##_expected[3]))                          \
    {                                                                      \
      if (DEBUG) 							   \
	{                                                                  \
	  printf("ERROR: vec_%s (float) expected value does not match\n",  \
		 #NAME);                                                   \
	  printf("   expected[0] = %f; result[0] = %f\n",                  \
		 f_##NAME##_expected[0], f_result[0]);                     \
	  printf("   expected[1] = %f; result[1] = %f\n",                  \
	         f_##NAME##_expected[1], f_result[1]);                     \
	  printf("   expected[2] = %f; result[2] = %f\n",                  \
	         f_##NAME##_expected[2], f_result[2]);                     \
	  printf("   expected[3] = %f; result[3] = %f\n",                  \
	         f_##NAME##_expected[3], f_result[3]);                     \
	}                                                                  \
      else                                                                 \
        abort();                                                           \
    }

#define DOUBLE_CHECK(NAME)			                              \
  d_result = vec_##NAME (d_src_a, d_src_b);				      \
  if ((d_result[0] != d_##NAME##_expected[0])				      \
      || (d_result[1] != d_##NAME##_expected[1]))                             \
    {                                                                         \
      if (DEBUG)							      \
	{                                                                     \
	  printf("ERROR: vec_%s(double) expected value does not match\n",     \
		 #NAME);						      \
	  printf("   expected[0] = %f; result[0] = %f\n",                     \
		 d_##NAME##_expected[0], d_result[0]);                        \
	  printf("   expected[1] = %f; result[1] = %f\n",                     \
		 d_##NAME##_expected[1], d_result[1]);                        \
	}                                                                     \
      else                                                                    \
	abort();                                                              \
    }

void abort (void);

void
float_tests (vector float f_src_a, vector float f_src_b,
	     vector float f_add_expected, vector float f_div_expected,
	     vector float f_max_expected, vector float f_min_expected,
	     vector float f_mul_expected, vector float f_sub_expected)
{
  /* These tests are fairly simple, the compiler will compute the ansers at
     compile time if the tests are not put into a function.   */
  vector float f_result;
  
  FLOAT_CHECK (add)
  FLOAT_CHECK (div)
  FLOAT_CHECK (max)
  FLOAT_CHECK (min)
  FLOAT_CHECK (mul)
  FLOAT_CHECK (sub)
}

void
double_tests (vector double d_src_a, vector double d_src_b,
	      vector double d_add_expected, vector double d_div_expected,
	      vector double d_max_expected, vector double d_min_expected,
	      vector double d_mul_expected, vector double d_sub_expected)
{
  /* These tests are fairly simple, the compiler will compute the ansers at
     compile time if the tests are not put into a function.   */
  vector double d_result;
  
  DOUBLE_CHECK (add)
  DOUBLE_CHECK (div)
  DOUBLE_CHECK (max)
  DOUBLE_CHECK (min)
  DOUBLE_CHECK (mul)
  DOUBLE_CHECK (sub)
}

int
main () {
  int i;
  vector float f_src_a = { 126.0, 23.0, -338.0, 17.0};
  vector float f_src_b = { 2.00, -4.0, 1.0, 4.0};
  vector float f_add_expected = { 128.0, 19.0, -337.0, 21.0};
  vector float f_div_expected = { 63.0, -5.75, -338, 4.25};
  vector float f_max_expected = { 126.0, 23.0, 1.0, 17.0};
  vector float f_min_expected = { 2.0, -4.0, -338.0, 4.0};
  vector float f_mul_expected = { 252, -92.0, -338, 68.0};
  vector float f_sub_expected = { 124.0, 27.0, -339.0, 13.0};

  vector double d_src_a = { 125.44, -338.56};
  vector double d_src_b = { 4.0, -2.0};
  vector double d_add_expected = { 129.44, -340.56};
  vector double d_div_expected = { 31.360000, 169.280000};
  vector double d_max_expected = { 125.44, -2.0};
  vector double d_min_expected = { 4.0, -338.56};
  vector double d_mul_expected = { 501.760000, 677.120000};
  vector double d_sub_expected = { 121.440000, -336.560000};

  /* Two argument float tests.  */
  float_tests (f_src_a, f_src_b, f_add_expected, f_div_expected, f_max_expected,
	       f_min_expected, f_mul_expected, f_sub_expected);

  /* Two argument double tests.  */
  double_tests (d_src_a, d_src_b, d_add_expected, d_div_expected,
		d_max_expected, d_min_expected, d_mul_expected, d_sub_expected);

  return 0;
}

/* { dg-final { scan-assembler-times {\mxvaddsp\M} 1 } } */
/* { dg-final { scan-assembler-times {\mxvdivsp\M} 1 } } */
/* { dg-final { scan-assembler-times {\mxvmulsp\M} 1 } } */
/* { dg-final { scan-assembler-times {\mxvsubsp\M} 1 } } */
/* { dg-final { scan-assembler-times {\mxvadddp\M} 1 } } */
/* { dg-final { scan-assembler-times {\mxvdivdp\M} 1 } } */
/* { dg-final { scan-assembler-times {\mxvmindp\M} 1 } } */
/* { dg-final { scan-assembler-times {\mxvmuldp\M} 1 } } */
/* { dg-final { scan-assembler-times {\mxvsubdp\M} 1 } } */

/* { dg-final { scan-assembler-times {\mxvmaxsp\M} 2 {target le} } } */
/* { dg-final { scan-assembler-times {\mxvmaxsp\M} 1 {target be} } } */

/* { dg-final { scan-assembler-times {\mxvminsp\M} 2 {target le} } } */
/* { dg-final { scan-assembler-times {\mxvminsp\M} 1 {target be} } } */

/* { dg-final { scan-assembler-times {\mxvmaxdp\M} 2 {target le} } } */
/* { dg-final { scan-assembler-times {\mxvmaxdp\M} 1 {target be} } } */
