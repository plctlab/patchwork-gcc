/* { dg-do run { target lp64 } } */
/* { dg-skip-if "" { powerpc*-*-darwin* } } */
/* { dg-options "-O2 -save-temps" } */

/* Functional test of the vector compare all builtins.  */

#include <altivec.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define DEBUG 0

#define FLOAT_1ARG_CHECK(NAME)                                                 \
  f_result = vec_##NAME (f_src_a);                                             \
                                                                               \
  if (f_result != f_##NAME##_expected)                                         \
    {                                                                          \
      if (DEBUG)                                                               \
	{                                                                      \
	  printf("ERROR: vec_%s (float) expected value does not match\n",      \
		 #NAME);                                                       \
	  printf("   expected = %d; result = %d\n",                            \
		 f_##NAME##_expected, f_result);                               \
	}                                                                      \
      else                                                                     \
	abort();                                                               \
    }

#define FLOAT_2ARG_CHECK(NAME)                                                 \
  f_result = vec_##NAME (f_src_a, f_src_b);                                    \
                                                                               \
  if (f_result != f_##NAME##_expected)                                         \
    {                                                                          \
      if (DEBUG)                                                               \
	{                                                                      \
	  printf("ERROR: vec_%s (float, float) expected value does not match\n", \
		 #NAME);                                                       \
	  printf("   expected = %d; result = %d\n",                            \
		 f_##NAME##_expected, f_result);                               \
	}                                                                      \
      else                                                                     \
	abort();                                                               \
    }
		 
#define DOUBLE_1ARG_CHECK(NAME)                                                \
  d_result = vec_##NAME (d_src_a);                                             \
                                                                               \
  if (d_result != d_##NAME##_expected)                                         \
    {                                                                          \
      if (DEBUG)                                                               \
	{                                                                      \
	  printf("ERROR: vec_%s (double) expected value does not match\n",     \
		 #NAME);                                                       \
	  printf("   expected = %d; result = %d\n",                            \
		 d_##NAME##_expected, d_result);                               \
	}                                                                      \
      else                                                                     \
	abort();                                                               \
    }

#define DOUBLE_2ARG_CHECK(NAME)                                                \
  d_result = vec_##NAME (d_src_a, d_src_b);                                    \
                                                                               \
  if (d_result != d_##NAME##_expected)                                         \
    {                                                                          \
      if (DEBUG)                                                               \
	{                                                                      \
	  printf("ERROR: vec_%s (double, double) expected value does not match\n", \
		 #NAME);                                                       \
	  printf("   expected = %d; result = %d\n",                            \
		 d_##NAME##_expected, d_result);                               \
	}                                                                      \
      else                                                                     \
	abort();                                                               \
    }
		 
void
float_tests (vector float f_src_a, vector float f_src_b, bool f_all_eq_expected,
	     bool f_all_gt_expected, bool f_all_ge_expected,
	     bool f_all_lt_expected, bool f_all_le_expected,
	     bool f_all_nan_expected, bool f_all_numeric_expected,
	     bool f_any_eq_expected, bool f_any_gt_expected,
	     bool f_any_ge_expected, bool f_any_lt_expected,
	     bool f_any_le_expected, bool f_any_nan_expected,
	     bool f_any_numeric_expected)
{
  /* Some of the tests are simple enough that the compiler computes the answer
     at compile time.  Put the tests in a function so the compiler will generate
     the code needed to perform the test.  */
  bool f_result;

  FLOAT_1ARG_CHECK (all_nan)
  FLOAT_1ARG_CHECK (all_numeric)
  FLOAT_1ARG_CHECK (any_nan)
  FLOAT_1ARG_CHECK (any_numeric)

  FLOAT_2ARG_CHECK (all_eq)
  FLOAT_2ARG_CHECK (all_gt)
  FLOAT_2ARG_CHECK (all_ge)
  FLOAT_2ARG_CHECK (all_lt)
  FLOAT_2ARG_CHECK (all_le)
  FLOAT_2ARG_CHECK (any_eq)
  FLOAT_2ARG_CHECK (any_gt)
  FLOAT_2ARG_CHECK (any_ge)
  FLOAT_2ARG_CHECK (any_lt)
  FLOAT_2ARG_CHECK (any_le)
}

void
double_tests (vector double d_src_a, vector double d_src_b,
	      bool d_all_eq_expected, bool d_all_gt_expected,
	      bool d_all_ge_expected, bool d_all_lt_expected,
	      bool d_all_le_expected, bool d_all_nan_expected,
	      bool d_all_numeric_expected, bool d_any_eq_expected,
	      bool d_any_gt_expected, bool d_any_ge_expected,
	      bool d_any_lt_expected, bool d_any_le_expected,
	      bool d_any_nan_expected, bool d_any_numeric_expected)
{
  /* Some of the tests are simple enough that the compiler computes the answer
     at compile time.  Put the tests in a function so the compiler will generate
     the code needed to perform the test.  */
  bool d_result;

  DOUBLE_1ARG_CHECK (all_nan)
  DOUBLE_1ARG_CHECK (all_numeric)
  DOUBLE_1ARG_CHECK (any_nan)
  DOUBLE_1ARG_CHECK (any_numeric)

  DOUBLE_2ARG_CHECK (all_eq)
  DOUBLE_2ARG_CHECK (all_gt)
  DOUBLE_2ARG_CHECK (all_ge)
  DOUBLE_2ARG_CHECK (all_lt)
  DOUBLE_2ARG_CHECK (all_le)
  DOUBLE_2ARG_CHECK (any_eq)
  DOUBLE_2ARG_CHECK (any_gt)
  DOUBLE_2ARG_CHECK (any_ge)
  DOUBLE_2ARG_CHECK (any_lt)
  DOUBLE_2ARG_CHECK (any_le)
}

void abort (void);

int
main () {
  int i;
  vector float f_src_a = {126.0, 23.0, -338.0, 17.0};
  vector float f_src_b = {2.00, 23.0, 1.0, 4.0};
  bool f_all_eq_expected = 0;
  bool f_all_gt_expected = 0;
  bool f_all_ge_expected = 0;
  bool f_all_lt_expected = 0;
  bool f_all_le_expected = 0;
  bool f_all_nan_expected = 0;
  bool f_all_numeric_expected = 1;
  bool f_any_eq_expected = 1;
  bool f_any_gt_expected = 1;
  bool f_any_ge_expected = 1;
  bool f_any_lt_expected = 1;
  bool f_any_le_expected = 1;
  bool f_any_nan_expected = 0;
  bool f_any_numeric_expected = 1;

  vector double d_src_a = { 125.44, -338.56};
  vector double d_src_b = d_src_a;
  bool d_result;
  bool d_all_eq_expected = 1;
  bool d_all_gt_expected = 0;
  bool d_all_ge_expected = 1;
  bool d_all_lt_expected = 0;
  bool d_all_le_expected = 1;
  bool d_all_nan_expected = 0;
  bool d_all_numeric_expected = 1;
  bool d_any_eq_expected = 1;
  bool d_any_gt_expected = 0;
  bool d_any_ge_expected = 1;
  bool d_any_lt_expected = 0;
  bool d_any_le_expected = 1;
  bool d_any_nan_expected = 0;
  bool d_any_numeric_expected = 1;

  /* Run tests.  */
  float_tests (f_src_a, f_src_b, f_all_eq_expected, f_all_gt_expected,
	       f_all_ge_expected, f_all_lt_expected, f_all_le_expected,
	       f_all_nan_expected, f_all_numeric_expected, f_any_eq_expected,
	       f_any_gt_expected, f_any_ge_expected, f_any_lt_expected,
	       f_any_le_expected, f_any_nan_expected, f_any_numeric_expected);

  double_tests (d_src_a, d_src_b, d_all_eq_expected, d_all_gt_expected,
		d_all_ge_expected, d_all_lt_expected, d_all_le_expected,
		d_all_nan_expected, d_all_numeric_expected, d_any_eq_expected,
		d_any_gt_expected, d_any_ge_expected, d_any_lt_expected,
		d_any_le_expected, d_any_nan_expected, d_any_numeric_expected);

  return 0;
}

/* { dg-final { scan-assembler-times {\mxvcmpeqsp\M} 4 } } */
/* { dg-final { scan-assembler-times {\mxvcmpgtsp\M} 4 } } */
/* { dg-final { scan-assembler-times {\mxvcmpgesp\M} 4 } } */
/* { dg-final { scan-assembler-times {\mxvcmpeqdp\M} 4 } } */
/* { dg-final { scan-assembler-times {\mxvcmpgedp\M} 4 } } */
/* { dg-final { scan-assembler-times {\mxvcmpgtdp\M} 4 } } */


